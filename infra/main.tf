provider "aws" {                        # Use the AWS provider
  region = "eu-central-1"               # Set AWS region to Frankfurt
}

resource "aws_security_group" "test_server_sg" {
  name        = "test-server-sg"
  description = "Allow HTTP and SSH"
  ingress {
    from_port   = 80
    to_port     = 80
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  ingress {
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  egress {
    from_port   = 0                               # Allow all outbound traffic
    to_port     = 0                               # Allow all outbound traffic
    protocol    = "-1"                            # All protocols
    cidr_blocks = ["0.0.0.0/0"]                   # To any IP address
  }
}

resource "aws_instance" "test_server" {
  ami             = "ami-04e601abe3e1a910f"         # Ubuntu 22.04 LTS AMI for eu-central-1
  instance_type   = "m5.large"
  security_groups = [aws_security_group.test_server_sg.name]
  user_data       = templatefile("${path.module}/user_data.sh.tpl", {
    SERVER_IMAGE = var.server_image
  })
  key_name        = "dev-machine-usage"

  tags = {
    Name = "test-server"
  }
}

output "public_ip" {
  value = aws_instance.test_server.public_ip
}