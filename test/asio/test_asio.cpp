#include <boost/asio.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <numeric>
#include <string_view>

namespace net = boost::asio;

constexpr auto operator""_KiB(unsigned long long x) -> unsigned long long {
    return x * 1024;
}

constexpr auto operator""_MiB(unsigned long long x) -> unsigned long long {
    return x * 1024 * 1024;
}

template <typename BufferSequence>
auto bufferseq_to_string_view(BufferSequence &bufseq) -> std::string_view {
    return std::string_view(static_cast<const char *>(bufseq.data()),
                            bufseq.size());
}

namespace {

struct StreamBufWithSizeLimit
    : public ::testing::TestWithParam<unsigned long long> {
    void SetUp() override {
        param = GetParam();
        sbuf = std::make_unique<net::streambuf>(param);
    }

    unsigned long long param;
    std::unique_ptr<net::streambuf> sbuf;
};

// https://www.boost.org/doc/libs/latest/doc/html/boost_asio/reference/streambuf.html
// says that, asio's current implementation of streambuf allocates a single
// contiguous block of memory.
TEST_P(StreamBufWithSizeLimit, AllocatesContiguousMemorySpace) {
    auto mutable_buffer_sequence = sbuf->prepare(param);
    auto sequence_len = std::distance(mutable_buffer_sequence.begin(),
                                      mutable_buffer_sequence.end());

    EXPECT_EQ(sequence_len, 1);
}

TEST_P(StreamBufWithSizeLimit, ThrowsLengthErrorWhenPrepareExceedsMaxSize) {
    EXPECT_THROW(sbuf->prepare(param * 2), std::length_error);
}

TEST_P(StreamBufWithSizeLimit, DoesntMoveMemoryForSmallPreparation) {
    auto out1 = sbuf->prepare(param);
    sbuf->commit(param);
    auto in1 = sbuf->data();
    sbuf->consume(param);

    auto out2 = sbuf->prepare(128);
    sbuf->commit(128);
    auto in2 = sbuf->data();
    sbuf->consume(64);

    auto out3 = sbuf->prepare(128);
    sbuf->commit(128);
    auto in3 = sbuf->data();
    sbuf->consume(64);

    EXPECT_EQ(out1.data(), in1.data());
    EXPECT_EQ(out2.data(), out1.data());
    EXPECT_GT(out3.data(), out2.data());
    EXPECT_GT(in3.data(), in2.data());
}

TEST_P(StreamBufWithSizeLimit, MovesInputForLargePreparation) {
    auto out1 = sbuf->prepare(param);
    sbuf->commit(param);
    sbuf->consume(param);
    auto out2 = sbuf->prepare(128);
    std::iota(static_cast<char *>(out2.data()),
              static_cast<char *>(out2.data()) + 128, 0);
    sbuf->commit(128);
    sbuf->consume(64); // consume 64 bytes only; 64 bytes of input remain

    auto out3 = sbuf->prepare(param - 64);
    sbuf->commit(param - 64);
    auto in = sbuf->data();

    EXPECT_EQ(in.data(), out1.data());
    EXPECT_EQ(static_cast<const char *>(in.data())[0], 64);
}

INSTANTIATE_TEST_SUITE_P(AStreamBuf, StreamBufWithSizeLimit,
                         ::testing::Values(1_KiB, 64_KiB, 4_MiB));

/*
 * `buffer_delta` is 128 in boost/asio/basic_streambuf.hpp
 */
TEST(AStreamBuf, HasExpectedCapacity) {
    net::streambuf sbuf(1_KiB);
    EXPECT_EQ(sbuf.capacity(), 128);
    EXPECT_EQ(net::streambuf(2_KiB).capacity(), 128);
}

TEST(ABufferSequence, SupportsDataAndSize) {
    net::streambuf sbuf;
    auto buffer_sequence = sbuf.prepare(0);
    EXPECT_NE(buffer_sequence.data(), nullptr);
    EXPECT_EQ(buffer_sequence.size(), 0);
}

TEST(AStreamBuf, DoesntReallocateForUndemandingPrepare) {
    net::streambuf sbuf;
    auto prev = sbuf.prepare(0);

    auto next = sbuf.prepare(sbuf.capacity());

    EXPECT_EQ(prev.data(), next.data());
}

TEST(AStreamBuf, ReallocatesForDemandingPrepare) {
    net::streambuf sbuf;
    auto prev = sbuf.prepare(0);

    auto next = sbuf.prepare(sbuf.capacity() * 2);

    EXPECT_NE(prev.data(), next.data());
}

TEST(AStreamBuf, CommitTransfersOutputToInput) {
    auto test_data = std::string("hello world");
    net::streambuf sbuf;

    auto output = sbuf.prepare(test_data.size());
    memcpy(output.data(), test_data.data(), test_data.size());
    sbuf.commit(test_data.size());
    auto input = sbuf.data();

    EXPECT_EQ(test_data.size(), sbuf.size());
    EXPECT_EQ(bufferseq_to_string_view(input), test_data);
    EXPECT_EQ(input.data(), output.data());
}

TEST(AStreamBuf, ConsumeHidesInputData) {
    auto test_data = std::string("hello world");
    net::streambuf sbuf;

    auto output = sbuf.prepare(test_data.size());
    memcpy(output.data(), test_data.data(), test_data.size());
    sbuf.commit(test_data.size());

    sbuf.consume(6);
    auto input = sbuf.data();

    EXPECT_EQ(sbuf.size(), test_data.size() - 6);
    EXPECT_EQ(bufferseq_to_string_view(input), "world");
    EXPECT_EQ(static_cast<const char *>(input.data()) -
                  static_cast<const char *>(output.data()),
              6);
}

} // namespace