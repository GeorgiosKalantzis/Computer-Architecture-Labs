# Computer Architecture Lab 1

## Ομάδα 11
### Καλαντζής Γεώργιος 8818 gkalantz@ece.auth.gr
### Κοσέογλου Σωκράτης 8837 Sokrkose@ece.auth.gr

Σκοπός της συγκεκριμένης εργασίας είναι μια πρώτη επαφή με τον _full-system simulator_ **gem5**, έτσι ώστε να κατανοήσουμε την αρχοτεκτονική αλλά και την λειτουργία ενός συστήματος. Στην συγκεκριμένη εργασία μελετάμε την την εκτέλεση προγραμμάτων τα οποία υποστηρίζονται απο επεξεργαστές με ARM ISA (Instruction Set Architectures). 

#### Ερώτημα 1

Στο 1ο ερώτημα εκτελέσαμε ένα απλό πρόγραμμα το οποίο εμφανίζει στην έξοδο του συστήματος (stdout) την έκφραση **Hello World!**.
Η εκτέλεση του προγράμματος γίνετα με την εντολή

`$ ./build/ARM/gem5.opt -d my_gem5_outputs/Hello_World_results configs/example/arm/starter_se.py --cpu="minor" "tests/test-progs/hello/bin/arm/linux/hello`

, δεδομένου ότι βρισκόμαστε στο _gem5 directory_. Η εντολή αυτή λεέι ότι θα κάνουμε build με τον simulator _gem5.opt_ με **guest architecture ARM**, στην συνέχεια χρησιμοποιόυμε το flag **-d** για να ορίσουμε το **stdout** του προγράμματος στο directory my_gem5_outputs/Hello_World_results. Στην συνέχεια δίνουμε το script το οποίο θέλουμε να τρέξουμε (_starter_se.py_) και για το οποίο θα μιλήσουμε στην συνέχεια. Τέλος, δίνουμε δύο **cmdargs** στο script, το ένα είναι ο τύπος cpu που θέλουμε να χρησιμοποιήσουμε (minor CPU) και το άλλο είναι το εκτελέσιμο binary αρχείο που θέλουμε να τρέξει το script (hello).

Ας δούμε λοιπόν πίο αναλυτικά τον κώδικα του script **starter_se.py**. Αρχικά, η πρώτη εντολή που εκτελείτε είναι η εξής εντολή:

```ruby
    parser = argparse.ArgumentParser(epilog=__doc__)
```

όπου η κλάση _argparce_ δημιουργεί το αντικείμενο _ArgumentParser_ το οποίο κρατάει τις εισόδους του script και τα δίνει στο αντικείμενο _parser_. Στην συνέχεια εκτελούνται οι παρακάτω εντολές,

```ruby
    parser.add_argument("commands_to_run", metavar="command(s)", nargs='*',
                        help="Command(s) to run")
    parser.add_argument("--cpu", type=str, choices=list(cpu_types.keys()),
                        default="atomic",
                        help="CPU model to use")
    parser.add_argument("--cpu-freq", type=str, default="4GHz")
    parser.add_argument("--num-cores", type=int, default=1,
                        help="Number of CPU cores")
    parser.add_argument("--mem-type", default="DDR3_1600_8x8",
                        choices=ObjectList.mem_list.get_names(),
                        help = "type of memory to use")
    parser.add_argument("--mem-channels", type=int, default=2,
                        help = "number of memory channels")
    parser.add_argument("--mem-ranks", type=int, default=None,
                        help = "number of memory ranks per channel")
    parser.add_argument("--mem-size", action="store", type=str,
                        default="2GB",
                        help="Specify the physical memory size")
```

Η διαδικασία ανάθεσης των cmdargs στις μεταβλητές του script γίνεται με την μέθοδο _add_argument()_ η οποία αναθέτει το binary εκτελέσιμο _hello_ και το cpy type _minor_. Βλέπουμε επίσης, ότι τα υπόλοιπα χαρακτηρηστικά του simulator όπως _CPU Frequency_, _Number of Cores_, _Memory_Type_ κλπ εφόσον δεν δίνονται σαν cmdargs παίρνουν τις _default_ τιμές τους. Πιο συγκεκριμένα, η default συχνότητα ρολογιού είναι **4GHz**, ο αριθμός των πυρήνων του επεξεργαστή είναι **1**, ενώ ο τύπος RAM είναι **DD3_1600_8X8** με 2 κανάλια (δηλ. **dual port**) και μέγεθος **2GB**. Όσον αφορά τον τύπο της cpu βλέπουμε ότι αναθέτετε ο τύπος _minor_ μέσω των παρακάτω εντολών

```ruby
    cpu_types = {
    "atomic" : ( AtomicSimpleCPU, None, None, None, None),
    "minor" : (MinorCPU,
               devices.L1I, devices.L1D,
               devices.WalkCache,
               devices.L2),
    "hpi" : ( HPI.HPI,
              HPI.HPI_ICache, HPI.HPI_DCache,
              HPI.HPI_WalkCache,
              HPI.HPI_L2)
}
```

και βλέπουμε ότι για κάθε τύπο cpu δίνονται και κάποια ορίσματα που αφορούν τις Level 1 και Level 2 Caches και πιο συγκεκριμένα μέσω του αρχείου _devices_ το οποίο έχουμε κάνει import αρχικοποιούνται οι Level 1 και Level 2 Instruction και Data Cache ως εξής:

```ruby
    class L1I(L1_ICache):
        tag_latency = 1
        data_latency = 1
        response_latency = 1
        mshrs = 4
        tgts_per_mshr = 8
        size = '48kB'
        assoc = 3


    class L1D(L1_DCache):
        tag_latency = 2
        data_latency = 2
        response_latency = 1
        mshrs = 16
        tgts_per_mshr = 16
        size = '32kB'
        assoc = 2
        write_buffers = 16


    class WalkCache(PageTableWalkerCache):
        tag_latency = 4
        data_latency = 4
        response_latency = 4
        mshrs = 6
        tgts_per_mshr = 8
        size = '1kB'
        assoc = 8
        write_buffers = 16


    class L2(L2Cache):
        tag_latency = 12
        data_latency = 12
        response_latency = 5
        mshrs = 32
        tgts_per_mshr = 8
        size = '1MB'
        assoc = 16
        write_buffers = 8
        clusivity='mostly_excl'
```

Στην συνέχεια εκτελείτε η εντολή

```ruby
    args = parser.parse_args()
```

η οποία ελέγχει τα cmdargs και μετατρέπει το κάθε argument σε σωστό τύπο μεταβλητής. Στην συνέχεια, εκτελείτε η εντολή

```ruby
   root = Root(full_system = False) 
```

η οποία δηλώνει το _Excecution Mode_ του _gem5_ ως **System-Call Emulation (SE)**, δηλαδή ορίζει ότι ο gem5 δεν τρέχει ένα πλήρες λειτουργικό σύστημα, παρά μόνο το πρόγραμμα που του δίνει ο χρήστης, οπότε κάνει emulate όλα τα system calls τα οποία θα προκύψουν. Αν είχαμε δώσει σαν όρισμα στην μέθοδο _Root_ ως _full_system = True_, τότε θα λέγαμε στον gem5 ότι θα κάνει emulate ένα πλήρες λειτουργικό σύστημα, δηλαδή θα ήταν σε _Excecution Mode_ **Full-System (FS)**. Στην συνέχεια, τρέχει η εντολή

```ruby
    root.system = create(args)
```

η οποία καλεί την μέθοδο _create_ η οποία με την σειρά της καλεί την **SimpleSystem()** η οποία αρχικοποιεί κάποιες παραμέτρους του συστήματος όπως το μέγεθος της γραμμής της cache αλλά, την τάση και την συχνότητα λειτουργίας του συστήματος καθώς και την τάση των Caches όπως φαίνονται παρακάτω:

```ruby
    # Use a fixed cache line size of 64 bytes
    cache_line_size = 64
```
```ruby
    # Create a voltage and clock domain for system components
    self.voltage_domain = VoltageDomain(voltage="3.3V")
    self.clk_domain = SrcClockDomain(clock="1GHz", voltage_domain=self.voltage_domain)
```

```ruby
    # Add CPUs to the system. A cluster of CPUs typically have
    # private L1 caches and a shared L2 cache.
    self.cpu_cluster = devices.CpuCluster(self, args.num_cores, args.cpu_freq, "1.2V", *cpu_types[args.cpu])
```

Ακόμη, η μέθοδος _create_ καλεί και την μέθοδο **get_processes()** η οποία παίρνει τα cmdargs και τα μεταφράζει ώς μια λίστα από processes. Τέλος, καλούνται οι μέθοδοι **m5.instansiate()** και **m5.simulate()** οι οποίες θεμελιώνουν την ιεραρχία της C++ και ξεκινούν την προσομοίωση αντίστοιχα.

### Συνοπτικά

|               | System Info   |
| ------------- | ------------- |
| Voltage       | 3.3V          |
| Frequency     | 1GHz          |

|               | CPU Info      |
| ------------- | ------------- |
| Num. of Cores | 1             |
| Frequency     | 4GHz          |
| Type          | Minor         |
| Voltage       | 1.2V          |

| Caches        | L1 Instr. Cache| L1 Data Cache  | L2 Cache        |
| ------------- | -------------  | -------------  | -------------   |
| Resp. latency | 1              | 1              | 5               |
| Tag   latency | 1              | 2              | 12              | 
| Data  latency | 1              | 2              | 12              |
| Associetivity | 3              | 2              | 16              | 
| Miss Stat. Reg.| 4             | 16             | 32              |
| Size          | 48kB           | 32kB           | 1MB             |

|               | RAM Info      |
| ------------- | ------------- |
| Memory        | 2GB           |
| Channels      | 2             |
| Type          | DDR3_1600_8x8 | 


#### Ερώτημα 2
#### A.
Όταν τελειώνει ο gem5 το simulation, κάνει export 3 αρχεία, το **config.ini**, το **config.json** και το **stats.txt**. Το αρχείο _config.ini_ περιέχει κάθε _Simulation Object_ (SimObject) που δημιουργήθηκε και τις παραμέτρους του. Το αρχείο _config.json_ περιέχει τις ίδιες πληροφορίες απλά σε .json μορφή. Ενώ, το _stats.txt_ περιέχει τις εξόδους και τα στατιστικά του simulation. Ας επιβεβαιώσουμε λοιπόν με βάση τα αποτελέσματα αυτά που θεωρήσαμε στους παραπάνω πίνακες. Αρχικά, στο config.ini file μπορόυμε να δούμε την μνήμη της κάθε γραμμής της cache αλλά και τα υπόλοιπα χαρακτηριστικά των Level 1 και Level 2 Caches καθώς και την τάση λειτουργίας του συστήματος.

```ruby
[system]
..
cache_line_size=64
..
[system.cpu_cluster.cpus.dcache]
..
assoc=2
data_latency=2
mshrs=16
response_latency=1
size=32768
tag_latency=2
..
[system.cpu_cluster.cpus.icache]
assoc=3
data_latency=1
mshrs=4
response_latency=1
size=49152
tag_latency=1
..
[system.cpu_cluster.l2]
assoc=16
data_latency=12
mshrs=32
response_latency=5
size=1048576
tag_latency=12
..
[system.voltage_domain]
type=VoltageDomain
eventq_index=0
voltage=3.3
```

Στην συνέχεια, στο **stats.txt** μπορούμε να επαληθεύσουμε την συχνότητα του συστήματος σε ticks (1 tick -> 1 picosec) καθώς και την συνότητα της CPU (Machine Cycle = 250ps). Επίσης μπορούμε να δούμε την τάση της CPU καθώς και τον χρόνο εκτέλεσης του simulation.

```ruby
---------- Begin Simulation Statistics ----------
final_tick                                   24321000 
host_seconds                                     0.13  
sim_freq                                 1000000000000 
sim_ticks                                    24321000 
..
system.cpu_cluster.clk_domain.clock               250 
..
system.cpu_cluster.voltage_domain.voltage     1.200000 
..
---------- End Simulation Statistics   ----------
```

#### B.

Στην συνέχεια θα μιλήσουμε για τον θεωρητικό υπολογισμό των **committed Instructions** σε σύγκριση με τα αποτελέσματα του gem5 simulation. Αρχικά, στο συγκεκριμένο παράδειγμα χρησιμοποιείται το μοντέλο Minor CPU, το οποίο δεν βασίζεται σε threads αλλά κάνει **pipelining**. Πιο συγκεκριμένα, αρχικά, κάνει _Fetch1_ για να κάνει fetch μια γραμμή από την cache, στην συνέχεια κάνει _fetch2_ ώστε να κάνει decomposition την γραμμή αυτή που περιέχει το instruction, έπειτα κάνει _decode_ το instruction σε Micro-Ops και τέλος είναι το _Excecution Stage_ όπου το Instruction γίνεται excecute. Εφόσον λοιπόν η διαδικασία αυτή γίνεται pipelinining, θα γίνεται ένα excecution(δηλ.**committed instructions**) ανά ένα κύκλο μηχανής, εκτός από το initial interval που είναι 3 κύκλοι μηχανής. Συνεπώς, ιδανικά, μπορούμε να γνωρίζουμε τις committed instructions απλά και μόνο γνωρίζοντας την συχνότητα του ρολογιού και τον χρόνο εκτέλεσης του simulation. Στην συγκεκριμένη περίπτωση, η συχνότητα του ρολογιού είναι 4GHz, δηλαδή ο κύκλος μηχανής είναι 0.25ns και το excecution time είναι 24321us, δηλαδή 97284 κύκλοι μηχανής. Συνεπώς θα έπρεπε οι committed instructions να είναι 97284 - 3 = **97281**.
Παρ' όλα αυτά η έξοδος του simulation μας λέει ότι πραγματοποιήθηκαν **5028** committed instructions και αυτό οφείλεται στο ότι υπήρξαν 86610 idle cycles τα οποία δημιουργούνται είτε από _misses_ των cache είτε λόγω read/write latency και έτσι το simulation εκτελεί 19.34 instructions pre cycle. Τα παραπάνω μπορούν να φανούν και στις εξής statistics:

```ruby
---------- Begin Simulation Statistics ----------
..
system.cpu_cluster.cpus.committedInsts           5028 
system.cpu_cluster.cpus.committedOps             5834  
system.cpu_cluster.cpus.discardedOps             1332 
system.cpu_cluster.cpus.cpi                 19.348449   
system.cpu_cluster.cpus.idleCycles              86610  
system.cpu_cluster.cpus.numCycles               97284   
..
```

#### Γ.

Σύμφωνα με τα statistics που μας έδωσε το simulation ο αριθμός προσπελάσεων της Level 2 Cache είναι **479** φορές και φαίνεται παρακάτω:

```ruby
system.cpu_cluster.l2.overall_accesses::total          479 
```


#### Ερώτημα 3

Ο emulator gem5 χρησιμοποιεί διάφορα μοντέλα CPU, κάποια in-οrder και κάποια out-of-order. Τα in-order μοντέλα που χρησιμοποιεί είναι τα **Simple CPU** και **Minor CPU** models.

Το μοντέλο Simple CPU έχει ανανεωθεί πρόσφατα και έχει χωριστεί σε 3 κλάσεις, **BaseSimpleCPU**, **AtomicSimpleCPU** και **TimingSimpleCPU**. 
Το BaseSimpleCPU μοντέλω εξυπηρετεί διάφορους σκοπούς όπως την εκτέλεση συναρτήσεων οι οποίες κάνουν έλεγχω για τυχόν interrupts, κάνει τα fetch requests και αναλαμβάνει και κάποιες post-excecute διεργασίες. Η κλάση BaseSimpleCPU δεν μπορεί να χρησιμοποιηθεί μόνη της καθώς χρειάζεται να κληθούν και μια από τις κλάσεις AtomicSimpleCPU ή TimingSimpleCPU οι οποίες κληρονομούν την κλάση BaseSimpleCPU. Όπως λέει και ο τίτλος το μοντέλο **AtomicSimpleCPU** χρησιμοποιεί _Atomic Memory Access_ όπου είναι μια αρκετά γρήγορη μέθοδος memory accessing, η οποία χρησιμοποιείται για fast forwarding και warming up των caches. Η κλάση AtomicSimpleCPU λοιπόν δημιουργεί ports μεταξύ μνήμης και επεξεργαστή. Η κλάση TimingSimpleCPU έχει ακριβώς την ίδια λειτουργία με την κλάση AtomicSimpleCPU (δηλαδή την διασύνδεση CPU-Cache) με μόνη διαφορά ότι χρησιμοποιεί _timing memory access_. Δηλαδή, μια μέθοδο προσπέλασης μνήμης η οποία είναι αρκετά λεπτομερής και κάνει stall τα cache accesses και περιμένει ACK (Acknowledgment) πρωτού συνεχίσει.

Το μοντέλο **MinorCPU** βασίζεται σε _pipeline_. Πιο συγκεκριμένα, αρχικά, κάνει _Fetch1_ για να κάνει fetch μια γραμμή από την cache, στην συνέχεια κάνει _fetch2_ ώστε να κάνει decomposition την γραμμή αυτή που περιέχει το instruction, έπειτα κάνει _decode_ το instruction σε Micro-Ops και τέλος είναι το _Excecution Stage_ όπου το Instruction γίνεται excecute.

#### A.

Για το συγκεκριμένο ερώτημα αναπτύχθηκε ένας κώδικας σε _C_ ο οποίος δημιουργεί δύο 2x2 πίνακες και εκτυπώνει τα αποτελέσματα του πολλαπλασιασμού τους. Στην συνέχεια φαίνεται η υλοποίση του, την οποία θα κάνουμε simulation με διάφορα μοντέλα CPUs ώστε να παρατηρήσουμε την συμπεριφορά και την ταχύτητα εκτέλεσης.

```ruby
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main()
{

	srand(time(0));

	int A[2][2];
	int B[2][2];
	int C[2][2];

	printf("-----------------------------------------------------------");
	printf("\nThe input tables are: \n\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			A[i][j] = rand() % 5;
			printf("%d\t", A[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			B[i][j] = rand() % 5;
			printf("%d\t", B[i][j]);
		}
		printf("\n");
	}

	printf("\nThe Multiplication of the tables is:\n\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			C[i][j] = 0;
			for(int k=0; k<2; k++){
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}

    for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			printf("%d\t", C[i][j]);
		}
		printf("\n");
	}

	printf("\n-----------------------------------------------------------\n");

}
```

Στην συνέχεια για να μπορέσουμε να κάνουμε compile το αρχείο .c σε εκτελέσιμο που προσδιορίζεται για ARM ISA έπρεπε να τρέξουμε την παρακάτω εντολή,

```ruby
    $ arm-linux-gnueabihf-gcc --static ./tests/MatrixMultiplication.c -o ./tests/MatrixMultiplicationCompiled
```

οπότε δημιουργείται το binary εκτελέσιμο αρχείο _MatrixMultiplicationCompiled_ στο directory _./tests/_

#### B.

Τώρα θα τρέξουμε το πρόγραμμα αυτό με δύο διαφορετικά μοντέλα CPU του gem5, το **MinorCPU** και το **TimingSimpleCPU**.

Τρέχοντας την παρακάτω εντολή εκτελούμε το script **se.py** και του δίνουμε ως ορίσματα το εκτελέσιμο του .c που δημιουργήσαμε, με το όρισμα --caches του λέμε να κάνει το simulation με **classic caches**, ορίζουμε ως CPU Model το **MinorCPU**, ορίζουμε συχνότητα ρολογιού **5GHz** και αποθηκεύουμε τα αποτελέσματα στο directory _MatrixMultiplication_MinorCPU_Results_.

```ruby
    $ ./build/ARM/gem5.opt -d my_gem5_outputs/MatrixMultiplication_MinorCPU_Results ./configs/example/se.py -c ./tests/MatrixMultiplicationCompiled --caches --cpu-type=MinorCPU --cpu-clock="5GHz"
```

Κάποια βασικά αποτελέσματα του simulation φαίνονται παρακάτω,

```ruby
final_tick                                   43108200                       # Number of ticks from beginning of simulation (restored from checkpoints and never reset)
host_inst_rate                                  84277                       # Simulator instruction rate (inst/s)
host_mem_usage                                 657000                       # Number of bytes of host memory used
host_op_rate                                   100463                       # Simulator op (including micro ops) rate (op/s)
host_seconds                                     0.42                       # Real time elapsed on the host
host_tick_rate                              102492820                       # Simulator tick rate (ticks/s)
sim_freq                                 1000000000000                       # Frequency of simulated ticks
sim_insts                                       35400                       # Number of instructions simulated
sim_ops                                         42239                       # Number of ops (including micro ops) simulated
sim_seconds                                  0.000043                       # Number of seconds simulated
sim_ticks                                    43108200                       # Number of ticks simulated
system.cpu.committedInsts                       35400                       # Number of instructions committed
system.cpu.committedOps                         42239                       # Number of ops (including micro ops) committed
system.cpu.cpi                               6.088729                       # CPI: cycles per instruction
system.cpu.discardedOps                          2576                       # Number of ops (including micro ops) which were discarded before commit
system.cpu.idleCycles                          164602                       # Total number of cycles that the object has spent stopped
system.cpu.ipc                               0.164238                       # IPC: instructions per cycle
system.cpu.numCycles                           215541                       # number of cpu cycles simulated
```

Στην συνέχεια εκτελώντας την παρακάτω εντολή τρέχουμε ακριβώς τα ίδια αντικείμενα, απλά σε διαφορετικό μοντέλο CPU, και πιο συγκεκριμένα στο μοντέλο **TimingSimpleCPU**

```ruby
    $ ./build/ARM/gem5.opt -d my_gem5_outputs/MatrixMultiplication_TimingSimpleCPU_Results ./configs/example/se.py -c ./tests/MatrixMultiplicationCompiled --caches --cpu-type=TimingSimpleCPU --cpu-clock="5GHz"
```

Κάποια βασικά αποτελέσματα του simulation φαίνονται παρακάτω,

```ruby
---------- Begin Simulation Statistics ----------
final_tick                                   50704000                       # Number of ticks from beginning of simulation (restored from checkpoints and never reset)
host_inst_rate                                  90398                       # Simulator instruction rate (inst/s)
host_mem_usage                                 655720                       # Number of bytes of host memory used
host_op_rate                                   107366                       # Simulator op (including micro ops) rate (op/s)
host_seconds                                     0.39                       # Real time elapsed on the host
host_tick_rate                              129639106                       # Simulator tick rate (ticks/s)
sim_freq                                 1000000000000                       # Frequency of simulated ticks
sim_insts                                       35298                       # Number of instructions simulated
sim_ops                                         41986                       # Number of ops (including micro ops) simulated
sim_seconds                                  0.000051                       # Number of seconds simulated
sim_ticks                                    50704000                       # Number of ticks simulated
system.cpu.Branches                              7529                       # Number of branches fetched
system.cpu.committedInsts                       35298                       # Number of instructions committed
system.cpu.committedOps                         41986                       # Number of ops (including micro ops) committed
system.cpu.idle_fraction                     0.000000                       # Percentage of idle cycles
system.cpu.not_idle_fraction                 1.000000                       # Percentage of non-idle cycles
system.cpu.numCycles                           253520                       # number of cpu cycles simulated
system.cpu.numWorkItemsCompleted                    0                       # number of work items this cpu completed
system.cpu.numWorkItemsStarted                      0                       # number of work items this cpu started
system.cpu.num_busy_cycles               253519.995000                       # Number of busy cycles
system.cpu.num_cc_register_reads               146533                       # number of times the CC registers were read
system.cpu.num_cc_register_writes               21634                       # number of times the CC registers were written
system.cpu.num_conditional_control_insts         5307                       # number of instructions that are conditional controls
```

Το πρώτο πράγμα το οποίο παρατηρούμε όταν συγκρίνουμε τα αποτελέσματα των δυο μοντέλων ο συνολικός αριθμός των κύκλων ρολογιού του κάθε simulation, δεδομένου ότι και τα δυο μοντέλα έτρεξαν στα 5MHz. Βλέπουμε ότι το μοντέλο _TiminigSimpleCPU_ εκτελείτε σε 253520 κύκλους μηχανής, ενώ το μοντέλο _MinorCPU_ εκτελείτε σε 215541, μόλις **37979** λιγότερους κύκλους μηχανής, δηλαδή εκτελείτε κατα **7.595 us** πιο γρήγορα. Το παραπάνω αποτέλεσμα είναι λογικό καθώς το μοντέλο _MinorCPU_ χρησιμοποιεί τεχνικές **pipelinining**.

TO-DO -> more similarities and differencies

#### Γ.




