FROM ubuntu:18.04 as websocket-chat-build

# Prepare build environment
RUN apt-get update && \
    apt-get -qq -y install gcc g++ ruby curl
RUN gem install Mxx_ru

RUN mkdir /tmp/websocket_chat
COPY externals.rb /tmp/websocket_chat
COPY dev /tmp/websocket_chat/dev

RUN echo "*** Building websocket_chat ***" \
    && cd /tmp/websocket_chat \
    && mxxruexternals \
	 && cd dev \
	 && MXX_RU_CPP_TOOLSET=gcc_linux ruby build.rb --mxx-cpp-release \
	 && mkdir /root/websocket_chat \
	 && cp target/release/websocket_chat_app /root/websocket_chat \
	 && cp chat_client.html /root/websocket_chat \
	 && cd /root \
	 && rm -rf /tmp/websocket_chat

FROM ubuntu:18.04 as websocket-chat

RUN apt-get update

COPY --from=websocket-chat-build /root/websocket_chat /root/websocket_chat

# Websocket_chat runs on port 8080.
EXPOSE 8080
WORKDIR /root/websocket_chat

# Start websocket_chat server
CMD ./websocket_chat_app 0.0.0.0 8080 .

