#!/bin/sh
# Release helper for make release.
set -eu

VERSION="${1:-}"
if test -z "$VERSION"; then
  echo "usage: $0 VERSION" >&2
  exit 1
fi

make check-release || exit 1

srcdir="$(sed -n "s/^srcdir='\\(.*\\)'$/\\1/p" config.status | head -n1)"
if test -z "$srcdir"; then
  echo "cannot determine srcdir from config.status" >&2
  exit 1
fi

if ! printf '%s\n' "$(VERSION)" | grep -Eq '^[0-9]+\.[0-9]+(\.[0-9]+)?$$'; then \
    echo "VERSION must be N.N or N.N.N, got: $(VERSION)"; \
    exit 1; \
    fi
if ! head -n 10 "$srcdir/NEWS" | grep -Eq "[Vv]ersion[[:space:]]+$VERSION([[:space:]]|[-:]|$$)"; then
  echo "NEWS first 10 lines must contain version $VERSION" >&2
  exit 1
fi
if ! head -n 1 "$srcdir/.appveyor.yml" | grep -Eq "^[[:space:]]*version:[[:space:]]*$VERSION([[:space:]]|$$|[.][{]build[}])"; then
  echo ".appveyor.yml first line must contain version: $VERSION" >&2
  exit 1
fi
if ! grep -Eq "^Version:[[:space:]]*$VERSION([[:space:]]|$$)" "$srcdir/libredwg.spec"; then
  echo "libredwg.spec Version: must be $VERSION" >&2
  exit 1
fi
if ! grep -Eq "^[[:space:]]*Release[[:space:]]+$VERSION([[:space:]]|$$)" "$srcdir/ChangeLog"; then
  echo "Latest 'Release $VERSION' commit missing in ChangeLog" >&2
  exit 1
fi

make -s -j check || exit 1

git tag -f "$(VERSION)" -s -m"release $(VERSION)"
cd "$srcdir" && sh autogen.sh
make regen-man

for f in examples/*.1 examples/*.5 programs/*.1; do
  if test -e "$f"; then
    git add -- "$f"
  fi
done
git commit --amend --no-edit -S

make dist || exit 1
make check-release || exit 1

git push
git push --tags
build-aux/gnupload --to ftp.gnu.org:libredwg "libredwg-${VERSION}.tar.gz" "libredwg-${VERSION}.tar.xz"
