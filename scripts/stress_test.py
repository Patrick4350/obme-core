import time
import random
import json
import subprocess
import sys
import os
from concurrent.futures import ThreadPoolExecutor
import threading

class OrderGenerator:
    def __init__(self, symbols=None):
        self.symbols = symbols or ["AAPL", "GOOGL", "MSFT", "TSLA", "AMZN"]
        self.order_id_counter = 1
        self.lock = threading.Lock()
    
    def generate_order(self):
        with self.lock:
            order_id = self.order_id_counter
            self.order_id_counter += 1
        
        return {
            "orderId": order_id,
            "clientId": random.randint(1, 1000),
            "symbol": random.choice(self.symbols),
            "type": random.choice(["LIMIT", "MARKET"]),
            "side": random.choice(["BUY", "SELL"]),
            "price": round(random.uniform(95.0, 105.0), 2),
            "quantity": random.randint(1, 1000),
            "timestamp": time.time()
        }
    
    def generate_burst(self, num_orders, burst_duration=1.0):
        """Generate a burst of orders over a specified duration"""
        orders = []
        start_time = time.time()
        
        for i in range(num_orders):
            order = self.generate_order()
            # Spread orders across the burst duration
            order["timestamp"] = start_time + (i * burst_duration / num_orders)
            orders.append(order)
        
        return orders

class StressTester:
    def __init__(self):
        self.generator = OrderGenerator()
        self.results = {
            "total_orders": 0,
            "total_time": 0,
            "orders_per_second": 0,
            "latency_stats": [],
            "errors": 0
        }
    
    def save_orders_to_file(self, orders, filename="../data/stress_test_orders.json"):
        """Save generated orders to a JSON file"""
        try:
            # Ensure data directory exists
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            with open(filename, 'w') as f:
                json.dump(orders, f, indent=2)
            print(f"Saved {len(orders)} orders to {filename}")
            return True
        except Exception as e:
            print(f"Error saving orders: {e}")
            return False
    
    def load_existing_orders(self, filename="../data/sample_orders.json"):
        """Load existing sample orders if available"""
        try:
            with open(filename, 'r') as f:
                return json.load(f)
        except Exception as e:
            print(f"Could not load existing orders: {e}")
            return None
    
    def simulate_high_frequency_trading(self, duration=60, orders_per_second=1000):
        """Simulate high-frequency trading scenario"""
        print(f"Simulating HFT for {duration} seconds at {orders_per_second} orders/sec")
        
        total_orders = duration * orders_per_second
        orders = []
        
        start_time = time.time()
        
        # Generate orders in batches for better performance
        batch_size = min(1000, orders_per_second)
        batches = total_orders // batch_size
        
        for batch in range(batches):
            batch_orders = []
            batch_start = time.time()
            
            for i in range(batch_size):
                order = self.generator.generate_order()
                order["batch"] = batch
                order["batch_index"] = i
                batch_orders.append(order)
            
            orders.extend(batch_orders)
            
            # Simulate processing time
            batch_time = time.time() - batch_start
            if batch_time < (batch_size / orders_per_second):
                time.sleep((batch_size / orders_per_second) - batch_time)
            
            if batch % 10 == 0:
                print(f"Generated batch {batch}/{batches} ({len(orders)} orders)")
        
        self.results["total_orders"] = len(orders)
        self.results["total_time"] = time.time() - start_time
        self.results["orders_per_second"] = len(orders) / self.results["total_time"]
        
        return orders
    
    def simulate_market_stress(self, num_orders=10000):
        """Simulate various market stress scenarios"""
        print(f"Simulating market stress with {num_orders} orders")
        
        scenarios = [
            ("Normal Trading", 0.7, lambda: self.generator.generate_order()),
            ("Price Volatility", 0.15, self.generate_volatile_order),
            ("Large Orders", 0.1, self.generate_large_order),
            ("Cancel Heavy", 0.05, self.generate_cancel_order)
        ]
        
        orders = []
        
        for i in range(num_orders):
            # Select scenario based on probability
            rand = random.random()
            cumulative = 0
            
            for scenario_name, probability, generator in scenarios:
                cumulative += probability
                if rand <= cumulative:
                    try:
                        order = generator()
                        order["scenario"] = scenario_name
                        orders.append(order)
                    except Exception as e:
                        print(f"Error generating order for {scenario_name}: {e}")
                        self.results["errors"] += 1
                    break
        
        return orders
    
    def generate_volatile_order(self):
        """Generate an order with volatile pricing"""
        order = self.generator.generate_order()
        # Add significant price volatility
        volatility = random.uniform(0.8, 1.2)
        order["price"] = round(order["price"] * volatility, 2)
        return order
    
    def generate_large_order(self):
        """Generate a large quantity order"""
        order = self.generator.generate_order()
        order["quantity"] = random.randint(10000, 100000)
        return order
    
    def generate_cancel_order(self):
        """Generate a cancel order"""
        order = self.generator.generate_order()
        order["type"] = "CANCEL"
        order["original_order_id"] = random.randint(1, self.generator.order_id_counter)
        return order
    
    def run_concurrent_test(self, num_threads=4, orders_per_thread=2500):
        """Run concurrent order generation test"""
        print(f"Running concurrent test with {num_threads} threads, {orders_per_thread} orders each")
        
        all_orders = []
        start_time = time.time()
        
        def worker_function(thread_id):
            thread_orders = []
            for i in range(orders_per_thread):
                order = self.generator.generate_order()
                order["thread_id"] = thread_id
                order["thread_order_index"] = i
                thread_orders.append(order)
            return thread_orders
        
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(worker_function, i) for i in range(num_threads)]
            
            for future in futures:
                try:
                    thread_orders = future.result(timeout=30)
                    all_orders.extend(thread_orders)
                except Exception as e:
                    print(f"Thread execution error: {e}")
                    self.results["errors"] += 1
        
        self.results["total_time"] = time.time() - start_time
        self.results["total_orders"] = len(all_orders)
        self.results["orders_per_second"] = len(all_orders) / self.results["total_time"]
        
        return all_orders
    
    def analyze_performance(self, orders):
        """Analyze the performance of generated orders"""
        if not orders:
            print("No orders to analyze")
            return
        
        print("\n=== Performance Analysis ===")
        print(f"Total Orders Generated: {len(orders)}")
        print(f"Total Time: {self.results['total_time']:.2f} seconds")
        print(f"Orders per Second: {self.results['orders_per_second']:.2f}")
        print(f"Errors: {self.results['errors']}")
        
        # Analyze order distribution
        symbols = {}
        sides = {"BUY": 0, "SELL": 0}
        types = {}
        
        for order in orders:
            symbol = order.get("symbol", "UNKNOWN")
            symbols[symbol] = symbols.get(symbol, 0) + 1
            
            side = order.get("side", "UNKNOWN")
            if side in sides:
                sides[side] += 1
            
            order_type = order.get("type", "UNKNOWN")
            types[order_type] = types.get(order_type, 0) + 1
        
        print(f"\nSymbol Distribution: {symbols}")
        print(f"Side Distribution: {sides}")
        print(f"Type Distribution: {types}")
        
        # Price analysis
        prices = [order.get("price", 0) for order in orders if order.get("price")]
        if prices:
            print(f"\nPrice Stats:")
            print(f"  Min: ${min(prices):.2f}")
            print(f"  Max: ${max(prices):.2f}")
            print(f"  Avg: ${sum(prices)/len(prices):.2f}")
        
        # Quantity analysis
        quantities = [order.get("quantity", 0) for order in orders if order.get("quantity")]
        if quantities:
            print(f"\nQuantity Stats:")
            print(f"  Min: {min(quantities)}")
            print(f"  Max: {max(quantities)}")
            print(f"  Avg: {sum(quantities)/len(quantities):.0f}")

def stress_test():
    """Main stress testing function"""
    print("=== OBME Core Stress Test ===")
    
    tester = StressTester()
    
    # Test scenarios
    test_scenarios = [
        ("Quick Test", lambda: tester.generator.generate_burst(100, 1.0)),
        ("High Frequency Simulation", lambda: tester.simulate_high_frequency_trading(10, 500)),
        ("Market Stress Test", lambda: tester.simulate_market_stress(5000)),
        ("Concurrent Load Test", lambda: tester.run_concurrent_test(4, 1000))
    ]
    
    print("Available test scenarios:")
    for i, (name, _) in enumerate(test_scenarios):
        print(f"  {i+1}. {name}")
    
    # Run all scenarios or let user choose
    try:
        choice = input("\nEnter scenario number (1-4) or 'all' to run all: ").strip().lower()
        
        if choice == 'all':
            all_orders = []
            for name, test_func in test_scenarios:
                print(f"\n--- Running {name} ---")
                orders = test_func()
                tester.analyze_performance(orders)
                
                # Save orders for this scenario
                filename = f"../data/stress_test_{name.lower().replace(' ', '_')}.json"
                tester.save_orders_to_file(orders, filename)
                
                all_orders.extend(orders)
            
            # Save combined results
            tester.save_orders_to_file(all_orders, "../data/stress_test_combined.json")
            
        elif choice.isdigit() and 1 <= int(choice) <= len(test_scenarios):
            scenario_index = int(choice) - 1
            name, test_func = test_scenarios[scenario_index]
            
            print(f"\n--- Running {name} ---")
            orders = test_func()
            tester.analyze_performance(orders)
            
            # Save orders
            filename = f"../data/stress_test_{name.lower().replace(' ', '_')}.json"
            tester.save_orders_to_file(orders, filename)
            
        else:
            print("Invalid choice. Running default quick test.")
            orders = tester.generator.generate_burst(1000, 2.0)
            tester.analyze_performance(orders)
            tester.save_orders_to_file(orders)
            
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
    except Exception as e:
        print(f"Error during testing: {e}")
    
    print("\nStress test completed!")

if __name__ == "__main__":
    stress_test()
