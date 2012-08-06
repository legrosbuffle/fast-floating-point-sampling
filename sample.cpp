/**
 * Author: Clement Courbet, clem@somebod.com
 *
 * This code is in the public domain. 
 *
 * Details: http://somebod.com/wp3/2012/08/fast-floating-point-sampling
 */

#include <vector>
#include <sys/time.h>
#include <iostream>
#include <cstdint>

class DummyValues {
public:
  DummyValues() : v(1) {}
  unsigned next() {
    v = (v + 1) & 0x0000001fU;
    return v;
  }
private:
  unsigned v;
};  

class ModSamplePredicate {
public:
  explicit ModSamplePredicate(unsigned modulo) : modulo(modulo) {}
  bool operator()(unsigned v) const {
    return !(v % modulo);
  }
private:
  unsigned modulo;
};

struct PowerOfTwoSamplePredicate {
  explicit PowerOfTwoSamplePredicate(unsigned l): k(((unsigned)1 << l) - 1) {}
  bool operator() (unsigned i) const {
    return !(i & k);
  }
private:
  unsigned k;
};

struct FloatSamplePredicate {
  explicit FloatSamplePredicate(float q): k((uint32_t)(0xffffffffU * q)) {
  }
  bool operator() (uint32_t i) const {
    return revBits(i) < k;
  }
private:
  //reverts the bits of v
  //(see http://graphics.stanford.edu/~seander/bithacks.html)
  static uint32_t revBits(uint32_t v) {
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = ( v >> 16             ) | ( v               << 16);
    return v;
  }
  uint32_t k;
};

template <class Sampler>
void bench(const Sampler& mustSample, unsigned numSamples) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int res = 0;
  DummyValues rnd;
  for (unsigned i = 0; i < numSamples; ++i) {
    if (mustSample(i)) {
      res += rnd.next();
    }
  }
  unsigned long start = (unsigned long) tv.tv_sec * 1000000 + tv.tv_usec;
  gettimeofday(&tv, NULL);
  unsigned long duration = ((unsigned long) tv.tv_sec * 1000000 + tv.tv_usec) - start; 
  std::cerr << "  res: " << res << " time=" << duration/1000 << " ms (" << numSamples / (double)duration << " M samples/sec)" << std::endl;
}

int main() {
  std::cerr << "ModSamplePredicate(1/16): " << std::endl;
  bench(ModSamplePredicate(16), 100000000);
  std::cerr << "PowerOfTwoSamplePredicate(1/16): " << std::endl;
  bench(PowerOfTwoSamplePredicate(4), 100000000);
  std::cerr << "FloatSamplePredicate(1/16): " << std::endl;
  bench(FloatSamplePredicate(1.0f/16), 100000000);
  std::cerr << "ModSamplePredicate(1/10): " << std::endl;
  bench(ModSamplePredicate(10), 100000000);
  std::cerr << "FloatSamplePredicate(1/10): " << std::endl;
  bench(FloatSamplePredicate(1.0f/10), 100000000);
  std::cerr << "FloatSamplePredicate(3.456%): " << std::endl;
  bench(FloatSamplePredicate(0.03456), 100000000);
}



