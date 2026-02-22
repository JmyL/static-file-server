provider "aws" { # Use the AWS provider
  region = var.region
}

data "aws_ami" "ubuntu" {
  most_recent = true

  filter {
    name   = "name"
    values = ["ubuntu/images/hvm-ssd-gp3/ubuntu-noble-24.04-amd64-server-*"]
  }

  owners = ["099720109477"] # Canonical
}

resource "aws_s3_bucket" "static_file_server" {
  bucket = var.s3_bucket
  force_destroy = true 

  tags = {
    Name        = "static-file-server"
  }
}

resource "aws_security_group" "test_server_sg" {
  name        = "test-server-sg"
  description = "Allow HTTP and SSH"
  ingress {
    from_port = 80
    to_port   = 80
    protocol  = "tcp"
    cidr_blocks = [
      # "10.0.0.0/16",   # Allow VPC communication
      "0.0.0.0/0" # To any IP address
    ]
  }
  ingress {
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  egress {
    from_port   = 0             # Allow all outbound traffic
    to_port     = 0             # Allow all outbound traffic
    protocol    = "-1"          # All protocols
    cidr_blocks = ["0.0.0.0/0"] # To any IP address
  }
}

resource "aws_instance" "test_server" {
  ami                  = data.aws_ami.ubuntu.id
  instance_type        = "m5.large"
  security_groups      = [aws_security_group.test_server_sg.name]
  user_data            = file("${path.module}/user_data.sh")
  iam_instance_profile = aws_iam_instance_profile.ssm_profile.name
  key_name             = "dev-machine-usage"

  tags = {
    Name = "test-server"
  }
}

resource "aws_instance" "k6_test" {
  ami                  = data.aws_ami.ubuntu.id
  instance_type        = "c5.large"
  security_groups      = [aws_security_group.test_server_sg.name]
  user_data            = file("${path.module}/user_data_load_test.sh")
  iam_instance_profile = aws_iam_instance_profile.ssm_profile.name

  tags = {
    Name = "k6-test"
  }
}

output "test_server_public_ip" {
  value = aws_instance.test_server.public_ip
}
output "test_server_instance_id" {
  value = aws_instance.test_server.id
}
output "k6_test_instance_id" {
  value = aws_instance.k6_test.id
}