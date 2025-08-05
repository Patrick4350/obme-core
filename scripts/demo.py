#!/usr/bin/env python3
"""
Demo script to showcase OBME Core functionality
"""

import sys
import os
import json

# Add the scripts directory to the path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'scripts'))

from stress_test import StressTester, OrderGenerator

def demo_order_generation():
    """Demonstrate order generation capabilities"""
    print("=== Order Generation Demo ===")
    
    generator = OrderGenerator(["DEMO", "TEST", "EXAMPLE"])
    
    # Generate a few sample orders
    print("Sample generated orders:")
    for i in range(5):
        order = generator.generate_order()
        print(f"  {i+1}. Order {order['orderId']}: {order['side']} {order['quantity']} {order['symbol']} @ {order['price']} ({order['type']})")
    
    print()

def demo_stress_scenarios():
    """Demonstrate different stress testing scenarios"""
    print("=== Stress Testing Scenarios Demo ===")
    
    tester = StressTester()
    
    # Quick burst test
    print("1. Quick Burst Test (50 orders in 0.5 seconds)")
    orders = tester.generator.generate_burst(50, 0.5)
    print(f"   Generated {len(orders)} orders")
    
    # Market stress test
    print("2. Market Stress Test (100 orders with different scenarios)")
    orders = tester.simulate_market_stress(100)
    print(f"   Generated {len(orders)} orders")
    
    # Analyze the results
    print("3. Performance Analysis:")
    tester.analyze_performance(orders)
    
    print()

def demo_json_output():
    """Demonstrate JSON order output"""
    print("=== JSON Output Demo ===")
    
    generator = OrderGenerator(["JSON", "DEMO"])
    orders = []
    
    for i in range(3):
        order = generator.generate_order()
        orders.append(order)
    
    print("Sample JSON orders:")
    print(json.dumps(orders, indent=2))
    print()

def main():
    """Main demo function"""
    print("OBME Core - Order Book Matching Engine Demo")
    print("=" * 50)
    
    try:
        demo_order_generation()
        demo_stress_scenarios()
        demo_json_output()
        
        print("Demo completed successfully!")
        print("\nTo run the full stress test, execute:")
        print("  python scripts/stress_test.py")
        print("\nTo run C++ tests (when compiled), execute:")
        print("  ./tests/orderbook_test.exe")
        
    except Exception as e:
        print(f"Demo error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
