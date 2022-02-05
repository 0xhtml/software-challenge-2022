#!/bin/sh
DOCKER_BUILDKIT=1 docker build --target out --output . .
