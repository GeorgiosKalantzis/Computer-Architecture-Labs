# Computer Architecture Lab 1

## Ομάδα 11
### Καλαντζής Γεώργιος ΑΕΜ Μειλ
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

Η διαδικασία ανάθεσης των cmdargs στις μεταβλητές του script γίνεται με την μέθοδο _add_argument()_ η οποία αναθέτει το binary εκτελέσιμο _hello_ και το cpy type _minor_. Βλέπουμε επίσης, ότι τα υπόλοιπα χαρακτηρηστικά του simulator όπως _CPU Frequency_, _Number of Cores_, _Memory_Type_ κλπ εφόσον δεν δίνονται σαν cmdargs παίρνουν τις _default_ τιμές τους. Πιο συγκεκριμένα, η default συχνότητα ρολογιού είναι **4GHz**. Στην συνέχεια εκτελείτε η εντολή

```ruby
    args = parser.parse_args()
```

η οποία ελέγχει τα cmdargs και μετατρέπει το κάθε argument σε σωστό τύπο μεταβλητής. Στην συνέχεια, εκτελείτε η εντολή

```ruby
   root = Root(full_system = False) 
```

η οποία δηλώνει το _Excecution Mode_ του _gem5_ ως **System-Call Emulation (SE)**, δηλαδή ορίζει ότι ο gem5 δεν τρέχει ένα πλήρες λειτουργικό σύστημα, παρά μόνο το πρόγραμμα που του δίνει ο χρήστης, οπότε κάνει emulate όλα τα system calls τα οποία θα προκύψουν. Αν είχαμε δώσει σαν όρισμα στην μέθοδο _Root_ ως _full_system = True_, τότε θα λέγαμε στον gem5 ότι θα κάνει emulate ένα πλήρες λειτουργικό σύστημα, δηλαδή θα ήταν σε _Excecution Mode_ **Full-System (FS)**.

