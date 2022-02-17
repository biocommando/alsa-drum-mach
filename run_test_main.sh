set -e
gcc --std=gnu99 test_main.c drum_mach.c -o test_main
./test_main

function verify_sum() {
    sha_sum=$(sha256sum $file | grep -o "[0-9a-f]*" | head -n -1)
    if [ "$sha_sum" != "$expected" ]
    then
        echo "ERROR: $file sha256 sum unexpected!"
        exit 1
    fi
    echo "File $file verified: $sha_sum = $expected"
}

echo "Verifying output file SHA256 sums"

expected="a8fc7330c0279f05f61174b839bfa20df8d90d35c196320a96b9ede3beb33ceb"
file="out_m.raw"
verify_sum
expected="9df751142379eda3c38ce05b9f9c4ba7e28e51f4bd5fb579c01fa85ea83d1bf7"
file="out_s.raw"
verify_sum

echo "SHA256 verifying done"
