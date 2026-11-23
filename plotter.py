import sys

def main():
    log_name = "simulation.log"
    steps = []
    pids = []
    tlb_events = []

    with open(log_name, "r") as f:
        for line in f:
            parts = [p.strip() for p in line.split("|")]
            if len(parts) >= 7:
                step = int(parts[0].split(":")[1].strip())
                pid = int(parts[1].split(":")[1].strip())
                tlb = parts[6].split(":")[1].strip()
                steps.append(step)
                pids.append(pid)
                tlb_events.append(tlb)

    print("Step\tPID\tTLB Status")
    for s, p, t in zip(steps, pids, tlb_events):
        bar = "#" * p
        print(f"{s}\tP{p} {bar}\t{t}")

if __name__ == "__main__":
    main()