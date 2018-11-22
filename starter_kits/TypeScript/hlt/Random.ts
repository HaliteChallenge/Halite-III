// Lehmer LCG 1951 - minimal standard RNG by Pakr-Miller http://www.firstpr.com.au/dsp/rand31/p1192-park.pdf

export class Random {
    private tempSeed: number;

    constructor(public seed = Math.random() * 32423534) {
        this.tempSeed = Math.floor(Math.abs(seed)) % 2147483647;
    }

    // Returns a PRN in range [0,1)
    public next() {
        this.tempSeed = this.tempSeed * 16807 % 2147483647; // PRN in [1,2147483646].
        return (this.tempSeed - 1) / 2147483646;
    }

    public *generate() {
        yield this.next();
    }

}
