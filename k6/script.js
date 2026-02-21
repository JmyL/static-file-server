import http from 'k6/http';
import { sleep, check } from 'k6';

export const options = {
    vus: 500,
    duration: '30s',
};

export default function() {
    const port = __ENV.TARGET_PORT || "8080";
    const ip = __ENV.TARGET_HOSTNAME || "127.0.0.1";
    const path = __ENV.DOWNLOAD_PATH || "README.md";
    const url = `http://${ip}:${port}/files/${path}`;

    let res = http.get(url);

    check(res, { "status is 200": (res) => res.status === 200 });
    sleep(0.01);
}