import http from 'k6/http';
import { sleep, check } from 'k6';

export const options = {
    vus: 5000,
    duration: '30s',
};

export default function() {
    const port = __ENV.PORT || "8080";
    let res = http.get(`http://127.0.0.1:${port}/files/README.md`);

    check(res, { "status is 200": (res) => res.status === 200 });
    sleep(0.01);
}