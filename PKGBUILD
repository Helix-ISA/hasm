pkgname=hasm
pkgver=0.1.0
pkgrel=2
pkgdesc="Helix Assembler"
arch=('x86_64')
license=('MIT')
depends=('glibc')
makedepends=('clang' 'make' 'git')

source=("git+https://github.com/Helix-ISA/hasm.git#branch=master")
sha256sums=('SKIP')

build() {
    cd "$srcdir/hasm"
    make
}

package() {
    cd "$srcdir/hasm"
    install -Dm755 bin/hasm "$pkgdir/usr/bin/hasm"
}
