pkgname=hasm
pkgver=0.1.0
pkgrel=2
pkgdesc="Helix Assembler"
arch=('x86_64')
license=('MIT')
depends=('glibc')
makedepends=('clang' 'make' 'git')

source=()
sha256sums=()

build() {
    make
}

package() {
    install -Dm755 bin/hasm "$pkgdir/usr/bin/hasm"
}
