#!/usr/bin/env sh

if [ -f $2/include/glad/glad.h ]
then
  true
else

  $1 -m venv .pyenv

  .pyenv/bin/pip install wheel
  .pyenv/bin/pip install glad

  .pyenv/bin/glad  \
    --profile core \
    --out-path $2  \
    --api gl=3.3   \
    --generator c  \
    --spec gl      \
    --reproducible \
    --no-loader
fi

