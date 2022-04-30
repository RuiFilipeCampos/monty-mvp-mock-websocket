FROM gcc:10


RUN apt-get update && apt-get install -y cmake


COPY server server

RUN cd server && ls


RUN cd server && cmake .
RUN cd server && cmake --build .
EXPOSE 8080

ENV PORT 8080

CMD cd server && ./server
