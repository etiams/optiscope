# Benchmarks

<details>
<summary>System information</summary>

```
                          ./+o+-       etiams@etiams
                  yyyyy- -yyyyyy+      OS: Ubuntu 24.04 noble
               ://+//////-yyyyyyo      Kernel: x86_64 Linux 6.8.0-60-generic
           .++ .:/++++++/-.+sss/`      Uptime: 16m
         .:++o:  /++++++++/:--:/-      Packages: 2799
        o:+o+:++.`..```.-/oo+++++/     Shell: bash 5.2.21
       .:+o:+o/.          `+sssoo+/    Resolution: 3840x2400
  .++/+:+oo+o:`             /sssooo.   DE: GNOME 46.7
 /+++//+:`oo+o               /::--:.   WM: Mutter
 \+/+o+++`o++o               ++////.   WM Theme: Adwaita
  .++.o+++oo+:`             /dddhhh.   GTK Theme: Yaru-red [GTK2/3]
       .+.o+oo:.          `oddhhhh+    Icon Theme: Yaru-red
        \+.++o+o``-````.:ohdhhhhh+     Font: Ubuntu Sans Bold 11 @wght=700
         `:o+++ `ohhhhhhhhyo++os:      Disk: 389G / 484G (85%)
           .o:`.syhhhhhhh/.oo++o`      CPU: AMD Ryzen 9 5900HX with Radeon Graphics @ 16x 4.68GHz
               /osyyyyyyo++ooo+++/     GPU: AMD/ATI Cezanne [Radeon Vega Series / Radeon Vega Mobile Series]
                   ````` +oo+++o\:     RAM: 5849MiB / 15388MiB
                          `oo++.
```

</details>

To observe the performance characteristics of optimal reduction à la Lambdascope, we present a number of benchmarks that expose different computational patterns.

On GNU/Linux, you need to reserve huge pages as follows: `sudo sysctl vm.nr_hugepages=6000`.

### [Fibonacci (native cells)](benchmarks/fibonacci-of-30.c)

Description: Recursively computes the 30th Fibonacci number using native cells & the built-in fixpoint operator.

```
Benchmark 1: ./fibonacci-of-30
  Time (mean ± σ):      5.879 s ±  0.021 s    [User: 5.691 s, System: 0.187 s]
  Range (min … max):    5.864 s …  5.914 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 17108681
Commutation interactions: 116357006
Beta interactions: 31
Native function calls: 12948453
If-then-elses: 4870845
Fixpoints: 31
Total interactions: 151285047
```

</details>

### [Fibonacci (Church numerals)](benchmarks/church-fibonacci-of-20.c)

Description: Recursively computes the 20th Fibonacci number using Church numerals & the standard Y combinator.

```
Benchmark 1: ./church-fibonacci-of-20
  Time (mean ± σ):     25.772 s ±  0.242 s    [User: 25.728 s, System: 0.043 s]
  Range (min … max):   25.436 s … 26.046 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 20464077
Commutation interactions: 2238539577
Beta interactions: 521833
Native function calls: 0
If-then-elses: 0
Fixpoints: 0
Total interactions: 2259525487
```

</details>

### [Church lists](benchmarks/church-list-reverse-and-sum.c)

Description: Reverses the Church-encoded list of 5000 cells & then sums all the cells up.

```
Benchmark 1: ./church-list-reverse-and-sum
  Time (mean ± σ):      4.922 s ±  0.058 s    [User: 4.854 s, System: 0.068 s]
  Range (min … max):    4.872 s …  5.021 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 25179997
Commutation interactions: 275405052
Beta interactions: 45004
Native function calls: 10000
If-then-elses: 0
Fixpoints: 0
Total interactions: 300640053
```

</details>

### [Scott list insertion sort](benchmarks/scott-insertion-sort.c)

Description: Performes an insertion sort on a Scott-encoded list of 150 cells, then sums all the cells up.

```
Benchmark 1: ./scott-insertion-sort
  Time (mean ± σ):      6.884 s ±  0.011 s    [User: 6.781 s, System: 0.103 s]
  Range (min … max):    6.873 s …  6.900 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 64777513
Commutation interactions: 370693967
Beta interactions: 46958
Native function calls: 22650
If-then-elses: 11175
Fixpoints: 452
Total interactions: 435552715
```

</details>

### [Scott list quicksort](benchmarks/scott-quicksort.c)

Description: Performes an (inefficient) quicksort on a Scott-encoded list of 100 cells, then sums all the cells up.

```
Benchmark 1: ./scott-quicksort
  Time (mean ± σ):     28.661 s ±  0.197 s    [User: 28.615 s, System: 0.047 s]
  Range (min … max):   28.428 s … 28.949 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 106108120
Commutation interactions: 1076209000
Beta interactions: 61814
Native function calls: 20000
If-then-elses: 9900
Fixpoints: 406
Total interactions: 1182409240
```

</details>

### [Scott trees](benchmarks/scott-tree-map-and-sum.c)

Description: Multiplies by 2 all the cells in a Scott-encoded binary tree of size 2^16, then sums all the cells up.

```
Benchmark 1: ./scott-tree-map-and-sum
  Time (mean ± σ):      4.029 s ±  0.037 s    [User: 3.967 s, System: 0.063 s]
  Range (min … max):    3.977 s …  4.066 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 40992285
Commutation interactions: 234842274
Beta interactions: 1048667
Native function calls: 262142
If-then-elses: 0
Fixpoints: 66
Total interactions: 277145434
```

</details>

### [Owl explosion](benchmarks/owl-explosion.c)

Description: Evaluates an application sequence of 5000 + 1 Owl combinators `(λa. λb. (b (a b)))`.

```
Benchmark 1: ./owl-explosion
  Time (mean ± σ):      3.416 s ±  0.008 s    [User: 3.227 s, System: 0.188 s]
  Range (min … max):    3.405 s …  3.425 s    5 runs
```

<details>
<summary>Interactions count</summary>

```
Annihilation interactions: 24985002
Commutation interactions: 124945006
Beta interactions: 9998
Native function calls: 0
If-then-elses: 0
Fixpoints: 0
Total interactions: 149940006
```

</details>
