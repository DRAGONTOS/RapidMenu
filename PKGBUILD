# Maintainer:  WorMzy Tykashi <wormzy.tykashi@gmail.com>
# Contributor: Dmitry Korzhevin <dkorzhevin AT gmail DOT com>
# Contributor: C.Coutinho <kikijump[at]gmail[dot]com>
# Contributor: Grigorios Bouzakis <grbzks[at]gmail[dot]com>
# Contributor: TDY <tdy@gmx.com>

pkgname=RapidMenu-git
_gitname=RapidMenu
pkgver=0.2.3
pkgrel=1
pkgdesc="A terminal multiplexer"
url="https://github.com/tmux/tmux/wiki"
arch=('x86_64')
license=('GPLv3')
depends=('tomlplusplus' 'cpptoml')
makedepends=('git' 'cmake' 'ninja')
provides=('RapidMenu')
conflicts=('RapidMenu')
source=('git+https://github.com/DRAGONTOS/RapidMenu.git')
md5sums=('SKIP')

prepare() {
  rm -rf build
}

build() {
  cd ${_gitname}
  cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -S . -B ./build -G Ninja
  cmake --build ./build
  chmod -R 777 ./build
}

package() {
  cd ${_gitname}
  sudo make install
}
