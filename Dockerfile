ARG BUILD_TYPE="Debug"

FROM archlinux/archlinux:base-devel AS buildenv

RUN pacman -Syu --noconfirm ninja                                                \
 && useradd -d /home/buildenv -m buildenv                                        \
 && echo 'buildenv ALL=(ALL) NOPASSWD: ALL' > /etc/sudoers.d/buildenv

USER buildenv

WORKDIR /home/buildenv

COPY --chown=buildenv:buildenv . infapi/

RUN curl --location --remote-name-all                                            \
  "https://aur.archlinux.org/cgit/aur.git/snapshot/mingw-w64-cmake.tar.gz"       \
  "https://aur.archlinux.org/cgit/aur.git/snapshot/mingw-w64-environment.tar.gz" \
  "https://aur.archlinux.org/cgit/aur.git/snapshot/mingw-w64-pkg-config.tar.gz"  \
 && cat mingw-w64-*.tar.gz | tar xvfz - -i                                       \
 && (cd mingw-w64-environment && makepkg -rsi --noconfirm)                       \
 && (cd mingw-w64-pkg-config && makepkg -rsi --noconfirm)                        \
 && (cd mingw-w64-cmake && makepkg -rsi --noconfirm)                             \
 && x86_64-w64-mingw32-cmake -S infapi -B infapi/build                           \
      -G Ninja                                                                   \
      -DBUILD_SHARED_LIBS:BOOL=OFF                                               \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE"                                           \
      -DRESTINIO_USE_EXTERNAL_EXPECTED_LITE=1                                    \
 && cmake --build infapi/build

FROM alpine

RUN apk add --no-cache wine-dev                                                  \
 && addgroup -S infapi                                                           \
 && adduser -S infapi -G infapi

USER infapi

WORKDIR /home/infapi

COPY --from=buildenv /home/buildenv/infapi/build/infapi.* .
COPY assets/docker/ .

CMD rm -rf ~/.wine && wine64 infapi.cmd