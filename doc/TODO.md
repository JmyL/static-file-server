- [ ] CMake Module: tool들을 optional로 변경
- [ ] 워크플로우 트리거를 maintainer(Write 권한자)로 제한
  - Settings → Actions → General → "Allow GitHub Actions to create and approve pull requests" → "Require approval for all outside collaborators" 등으로 제한 가능
- [ ] Secrets 저장
    - AWS_ACCESS_KEY_ID
    - AWS_SECRET_ACCESS_KEY
    - K6_CLOUD_TOKEN
    - SSH_PRIVATE_KEY

- `terraform init`
- `terraform plan`
- `terraform apply`
- `ssh -i ~/.ssh/dev-machine-usage.pem ubuntu@<ip_address>`
- `terraform destroy`

terraform output -raw public_ip : ip 받기