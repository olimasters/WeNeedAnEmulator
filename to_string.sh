#!/bin/bash

echo $1 | sed "s/\(.\)/'\1',/g" | rev | cut -c2- | rev
