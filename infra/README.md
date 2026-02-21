# Infrastructure

## SSM

You can start interactive session with `aws ssm start-session --target <instance-id>`.
Run command and check results with:

```sh
aws ssm send-command \
  --instance-ids <instance-id> \
  --document-name "AWS-RunShellScript" \
  --parameters 'commands=["echo hello world"]'
# above command returns a command ID
aws ssm list-command-invocations \
  --command-id <command-id> \
  --details
```

## 