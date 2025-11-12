#!/bin/bash
echo "[INFO] Installing assets from https://github.com/Matero952/h12_sim_assets.git"
cd "$1"
git clone https://github.com/Matero952/h12_sim_assets.git
echo "[INFO] Assets installed at $1"
