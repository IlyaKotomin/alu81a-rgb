# Maintainer: kotoxik <mostlys4nny@gmail.com>
pkgname=alu81a-rgb
pkgver=1.0.0
pkgrel=1
pkgdesc="CLI RGB controller for Dark Project Terra Nova/Nostra (ALU81A) keyboard via hidraw"
arch=('x86_64' 'aarch64')
url="https://github.com/kotoxik/alu81a-rgb"
license=('MIT')
depends=('glibc')
makedepends=('gcc')
source=("$pkgname-$pkgver.tar.gz::$url/archive/v$pkgver.tar.gz")
sha256sums=('c86d8ca7faf2fd4ed899c33258c4b278b251ca30c2cce11db3b1ca536c6c3734')

build() {
    cd "$pkgname-$pkgver"
    make
}

package() {
    cd "$pkgname-$pkgver"
    make DESTDIR="$pkgdir" PREFIX=/usr install
}
