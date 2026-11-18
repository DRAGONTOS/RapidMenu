pkgname=RapidMenu-git
_gitname=RapidMenu
pkgver=1.0.0
pkgrel=1
pkgdesc="Utilize configuration files to create runner scripts."
url="https://github.com/DRAGONTOS/RapidMenu"
arch=('x86_64')
license=('GPLv3')
makedepends=('git' 'cmake' 'ninja' 'cpptoml')
provides=('RapidMenu')
conflicts=('RapidMenu')
source=('git+https://github.com/DRAGONTOS/RapidMenu.git')
md5sums=('SKIP')

pkgver() {
    cd ${_gitname}
    sed -n 's/.*"version": "\([^"]*\)".*/\1/p' props.json
}

prepare() {
    rm -rf build
}

build() {
    cd ${_gitname}
    cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -S . -B ./build -G Ninja
    cmake --build ./build
}

package() {
    cd ${_gitname}
    install -Dm755 ./build/RapidMenu ${pkgdir}/usr/bin/RapidMenu
    install -Dm644 assets/dashboard.conf ${pkgdir}/usr/share/doc/RapidMenu/examples/dashboard.conf
}
