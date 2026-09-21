pkgname=hasm
pkgver=0.1.0
pkgrel=2
pkgdesc="Helix Assembler"
arch=('x86_64')
license=('MIT')
depends=('glibc')
makedepends=('clang' 'make' 'git')

source=("https://github.com/Helix-ISA/hasm/archive/refs/heads/master.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/hasm-master"
    make
}

package() {
    cd "$srcdir/hasm-master"
    install -Dm755 bin/hasm "$pkgdir/usr/bin/hasm"
}
