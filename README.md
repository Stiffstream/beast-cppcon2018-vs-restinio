# What Is "Beast-CppCon2018-vs-RESTinio"?

Vinnie Falco, the main author of Boost.Beast, gives a talk on CppCon-2018: [Get Rich Quick! Using Boost.Beast WebSockets and Networking TS](https://cppcon2018.sched.com/event/FnJn/get-rich-quick-using-boostbeast-websockets-and-networking-ts). This talk is about modern C++, upcoming Networking TS and Boost.Beast library.

A simple example was prepared by Vinnie Falco for that talk. Source code of that example can be found on [GitHub](https://github.com/vinniefalco/CppCon2018).

Despite the fact that Boost.Beast is a great library and is bright example of C++ masterpiece, we think that usage of Boost.Beast for simple tasks is an overkill. Simple things must be done much simpler than approach proposed by Boost.Beast.

To prove our point of view we have created a reimplementation of Vinnie's example using OpenSource [RESTinio](https://stiffstream.com/en/products/restinio.html) library from [stiffstream](https://stiffstream.com). This repository contains the source code of our version of that simple application.

# Some Notes To Readers

We do not have a goal to create a side-by-side comparison between Beast-based and RESTinio-based solutions. This comparison is left as an exercise for a reader :)

Seriously speaking the best way to feel the difference is to study Beast-based solution and then compare it with RESTinio-based one. Look to the code. For example, look at [the most important part of main() function in Vinnie's code](https://github.com/vinniefalco/CppCon2018/blob/cfa005305bf4b4e42ea886a6afd989d057f69605/main.cpp#L40-L60). And then to [the same part in our code](https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio/src/fb97378b1f839c1adbe35bb7de491a1857bf6877/dev/websocket_chat/main.cpp#lines-68:221).

You can also take a look at request-processing code. In [Beast-based solution](https://github.com/vinniefalco/CppCon2018/blob/cfa005305bf4b4e42ea886a6afd989d057f69605/http_session.cpp#L83-L293). And then in [our solution](https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio/src/fb97378b1f839c1adbe35bb7de491a1857bf6877/dev/websocket_chat/main.cpp#lines-234:247).

## What Do We Think?

We think that Boost.Beast is a great library. But it is too low-level.

Because of that we see only two niches appropriate for Boost.Beast:

1. High-performant systems where you need a full control on everything. You can rely on Boost.Beast functionality and customization abilities to get maximum performance.
2. More high-level and user-friendly libraries for working with HTTP in C++ code. Boost.Beast can be used as basis for that libraries. And you can hide Beast's complexity under your high-level API.

But if you need to solve more or less simple tasks outside of those niches it could be easier to use other C++ libraries like [RESTinio](https://stiffstream.com/en/products/restinio.html), [C++REST SDK](https://github.com/Microsoft/cpprestsdk), [RestBed](https://github.com/Corvusoft/restbed), [Pistache](https://github.com/oktal/pistache), [served](https://github.com/meltwater/served),  [CROW](https://github.com/ipkn/crow) and so on.

# Obtaining And Building

## Prerequisites

A C++ compiler with C++14 support is required.

## Obtaining Sources

Just use` hg clone` command to download example's sources.

```{.sh}
hg clone https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio
```

You can also download a tarball with example's sources from a [Downloads](https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio/downloads/) section. That tarball already contains all necessary dependencies.

## Building

There are three approaches for building Shrimp:

* by using Docker;
* by using CMake;
* by using Mxx_ru.

The simplest one is Docker-build because all necessary dependencies are downloaded and built automatically during docker build command.

### Docker build

You need [Docker](https://www.docker.com/) installed.

Enter into example's folder and run docker build command:

```{.sh}
cd beast-cppcon-2018-vs-restinio
docker build -t websocket_chat .
```

All necessary dependencies will be downloaded, configured and built inside Docker's image.

Then you can start Docker's image:

```{.sh}
docker run -p 8080:8080 websocket_chat
```

### Build with CMake

CMake version 3.8.0 or above is required.

The full source code of the example with all necessary dependencies must be downloaded as tarball from [Downloads](https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio/downloads/) section. Then (_XYZ_ is a precise version of tarball):

```{.sh}
tar xaf beast-cppcon2018-vs-restinio-XYZ.tar.bz2
cd beast-cppcon2018-vs-restinio/dev
mkdir cmake_build
cd cmake_build
cmake -DCMAKE_INSTALL_PREFIX=target ..
cmake --build . --config Release
cmake --build . --config Release --target install
```

`websocket_chat_app` will be copied to `target/bin` subfolder.

### Build with Mxx_ru

To build the example from repository you need to install Mxx_ru 1.6.14.5 or above:

```{.sh}
apt-get install ruby
gem install Mxx_ru
```

Mxx_ru helps to download necessary dependencies and used as a build tool.

Clone repo and download dependencies:

```{.sh}
hg clone https://bitbucket.org/sobjectizerteam/beast-cppcon-2018-vs-restinio
cd beast-cppcon-2018-vs-restinio
mxxruexternals
```

Once you get example's sources and its dependencies you can build it.

```{.sh}
# Start with repository root directory
cd dev
# Build.
ruby build.rb --mxx-cpp-release
# See websocket_chat_app in 'target/release' directory.
```

Or you can download a tarball with all dependencies from from [Downloads](https://bitbucket.org/sobjectizerteam/beast-cppcon2018-vs-restinio/downloads/) section. Then (_XYZ_ is a precise version of tarball):

```{.sh}
tar xaf beast-cppcon2018-vs-restinio-XYZ.tar.bz2
cd beast-cppcon2018-vs-restinio/dev
ruby build.rb --mxx-cpp-release
```
`websocket_chat_app` will be in `target/release` subfolder.

# License

The example is distributed under BSD-3-CLAUSE license.

For the license of RESTinio library see LICENSE file in RESTinio distributive.

For the license of asio library see COPYING file in asio distributive.

For the license of nodejs/http-parser library see LICENSE file in nodejs/http-parser distributive.

For the license of fmtlib see LICENSE file in fmtlib distributive.
