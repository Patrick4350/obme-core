# Example Dockerfile for OBME Core
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y g++ cmake make python3
WORKDIR /app
COPY . .
RUN cmake . && make
CMD ["./obme-core"]
