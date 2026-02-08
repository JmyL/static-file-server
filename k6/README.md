You can run benchmark like below.

Benchmarking Nginx:
```bash
PORT=8080 k6 run script.js
```

Benchmarking our server:
```bash
PORT=8081 k6 run script.js
```