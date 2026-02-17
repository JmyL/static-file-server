resource "aws_sns_topic" "ec2_alarm" {
  name = "ec2-alarm-topic"
}

resource "aws_sns_topic_subscription" "email" {
  topic_arn = aws_sns_topic.ec2_alarm.arn
  protocol  = "email"
  endpoint  = "jmyl@me.com"
}

resource "aws_cloudwatch_metric_alarm" "ec2_running_too_long" {
  alarm_name          = "EC2RunningTooLong"
  comparison_operator = "GreaterThanThreshold"
  evaluation_periods  = 1
  metric_name         = "StatusCheckFailed_System"
  namespace           = "AWS/EC2"
  period              = 300 # 5 minutes
  statistic           = "Maximum"
  threshold           = 0
  alarm_description   = "EC2 instance running too long"
  actions_enabled     = true
  alarm_actions       = [aws_sns_topic.ec2_alarm.arn]
  dimensions = {
    InstanceId = aws_instance.test_server.id
  }
}

