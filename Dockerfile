FROM gcc:9

COPY server server

RUN apt-get update && apt-get install -y cmake

RUN cd server && cmake .
RUN cd server && cmake --build .
EXPOSE 8080

ENV PORT 8080

CMD cd server && ./server
