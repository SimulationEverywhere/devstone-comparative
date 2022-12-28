import subprocess


if __name__ == '__main__':
    a = "adevs,xdevs-c,xdevs-rs,xdevs-cpp,xdevs-java-sequential"
    models = [['LI', 'HI', 'HO'], ['HOmod']]
    sizes = [list(range(20, 401, 20)), list(range(5, 51, 5))]
    for i, models in enumerate(models):
        for m in models:
            p = []
            p.extend([f'{m}-{w}-{d}-0-0' for w in sizes[i] for d in sizes[i]])
            p = ','.join(p)
            n = 30
            o = f"results_{m}.csv"

            cmd = f"python3 devstone_comparative.py -a {a} -p {p} -n {n} -o {o}"
            subprocess.run(cmd.split())
