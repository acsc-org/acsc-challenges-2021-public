# Solution

## Step 1. Parse LPC data

```
for i in `seq 1 5`
do
    ./alldata.py capture0${i}.csv > capture0${i}_alldata.txt
done
```

## Step 2. Carve out VMK-like bytes

VMK has known pattern `"2c 00 00 00 ......"` in its header.

* <https://pulsesecurity.co.nz/articles/TPM-sniffing>
* <https://labs.f-secure.com/blog/sniff-there-leaks-my-bitlocker-key/>

Seek the pattern in parsed data and carve it out.

```
capture01 2c000000010000XXff200000XX14XXXX657e9b102edfXXXXXXXXXXXX5073XXb2cccaXX5423XXXX0b1cXX84XX
capture02 2c0000XX010000XXff200000XX14XX60657e9bXX2edffeXXXX9685705073XXXXcccaXXXX23a78f0b1dXXXXXX
capture03 XXXX00XX010000XXXX20XX00XXXXfc6065XX9bXXXXXXfe3d4d96XX705073XXXXccccXXXX23a78f0b1cXXXX09
capture04 2c00XX0001XX0000ffXX00008eXXXX60657eXX12XXdffe3dXXXX85XX507307b2XXca3654XXa78f0b1cdb8409
capture05 XX000000ff000000ff2000008eXXfc60XX7e9b12XXXXXX3d4d9685f0507307XXXXcc365423XXXX0b1cdfXXXX
```

## Step 3. Analyse the carved bytes statistically

```
2c00000001000000ff2000008e14fc60657e9b122edffe3d4d968570507307b2ccca365423a78f0b1c??8409
```

## Step 4. Bruteforce the undetermined byte

```
8e14fc60657e9b122edffe3d4d968570507307b2ccca365423a78f0b1cdb8409
```
