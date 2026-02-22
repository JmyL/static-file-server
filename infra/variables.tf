variable "region" {
  description = "Region where the EC2 instances will be created"
  type        = string
  default     = "eu-central-1"
}

variable "s3_bucket" {
  description = "S3 bucket name for saving artifacts"
  type        = string
  default     = "jmyl-tmp-files"
}