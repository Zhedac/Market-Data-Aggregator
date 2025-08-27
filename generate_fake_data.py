import os
import random
from datetime import datetime, timedelta
from faker import Faker
import argparse

# Configuration
DEFAULT_NUM_ROWS_PER_FILE = 100000
DEFAULT_NUM_NEW_FILES = 1000
EXISTING_SYMBOLS = ["AAPL", "CSCO", "MSFT"]
EXCHANGES = ["NASDAQ", "NYSE", "LSE", "BATS", "ARCA"]
TRADE_TYPES = ["TRADE", "QUOTE"]

fake = Faker()

def generate_random_symbol(length=3):
    """Generates a random uppercase symbol."""
    return ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=length))

def generate_market_data_file(symbol, output_dir, num_rows):
    """Generates a single market data file."""
    file_path = os.path.join(output_dir, f"{symbol}.txt")
    print(f"Generating {num_rows} rows for {symbol}.txt...")

    start_time = datetime(2024, 1, 1, 9, 0, 0, 0) # Start from Jan 1, 2024, 9 AM

    with open(file_path, 'w') as f:
        # Write header
        f.write("timestamp,price,size,exchange,type\n")

        for i in range(num_rows):
            # Increment timestamp by a random small delta (1 to 100 milliseconds)
            start_time += timedelta(microseconds=random.randint(1000, 100000)) # 1ms to 100ms
            timestamp_str = start_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] # Milliseconds precision

            price = round(random.uniform(50.0, 500.0), 2)
            size = random.randint(100, 5000)
            exchange = random.choice(EXCHANGES)
            trade_type = random.choice(TRADE_TYPES)

            f.write(f"{timestamp_str},{price:.2f},{size},{exchange},{trade_type}\n")
    print(f"Finished generating {symbol}.txt")

def main():
    parser = argparse.ArgumentParser(description="Generate market data files.")
    parser.add_argument("--rows", type=int, default=DEFAULT_NUM_ROWS_PER_FILE,
                        help=f"Number of rows per file. Defaults to {DEFAULT_NUM_ROWS_PER_FILE}.")
    parser.add_argument("--files", type=int, default=DEFAULT_NUM_NEW_FILES,
                        help=f"Number of new files to generate. Defaults to {DEFAULT_NUM_NEW_FILES}.")
    parser.add_argument("--output_dir", type=str, default=None,
                        help="Output directory path. If not provided, a 'trades' directory will be created in the current location.")

    args = parser.parse_args()

    num_rows_per_file = args.rows
    num_new_files = args.files
    output_dir = args.output_dir

    if output_dir is None:
        output_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "trades")    

    # Ensure the output directory exists
    os.makedirs(output_dir, exist_ok=True)

    # Generate new unique symbols
    new_symbols = set()
    while len(new_symbols) < num_new_files:
        new_symbols.add(generate_random_symbol())

    all_symbols = list(EXISTING_SYMBOLS) + list(new_symbols)

    # Generate files for all symbols
    for symbol in all_symbols:
        generate_market_data_file(symbol, output_dir, num_rows_per_file)

    print("\nAll market data files generated successfully!")

if __name__ == "__main__":
    main()