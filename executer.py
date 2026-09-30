import subprocess
from google import genai
from google.genai import types
import json

native_json_schema = {
    "type": "object",
    "properties": {
        "action1": {"type": "string", "enum": ["create", "clear", "pass"]},
        "target1": {"type": "string", "enum": ["name", "age", "country"]},
        "action2": {"type": "string", "enum": ["create", "clear", "pass"]},
        "target2": {"type": "string", "enum": ["name", "age", "country"]}
    },
    "required": ["action1", "target1", "action2", "target2"]
}

client = genai.Client()

def main():
    process = subprocess.Popen(
        ["./database"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True
    )

    name_vis, age_vis, country_vis = 0, 0, 0
    name_q, age_q, country_q = 0, 0, 0
    active_graphs = []

    print("Started Python program")

    while True:
        line = process.stdout.readline()
        if not line:
            print("The database program execution ended.")
            break
        
        clean_line = line.strip() 

        if not clean_line.startswith("name_vis") and not clean_line.startswith("name_q_ct") and not clean_line.startswith("current graphs"):
            continue

        if clean_line.startswith("name_vis:"):
            tokens = clean_line.split()
            name_vis = int(tokens[1])     
            age_vis = int(tokens[3])      
            country_vis = int(tokens[5])  

        elif clean_line.startswith("name_q_ct:"):
            tokens = clean_line.split()
            name_q = int(tokens[1])
            age_q = int(tokens[3])
            country_q = int(tokens[5])

        elif clean_line.startswith("current graphs:"):
            tokens = clean_line.split()
            active_graphs = tokens[2:]
            
            system_instruction = """
            You are an expert Autonomous Database Administrator (DBA). Your job is to analyze real-time performance metrics and optimize index allocations to minimize 'Index Hops' (table scans).

            CRITICAL STRATEGY & CONSTRAINTS:
            1. MAXIMUM INDEXES: You can have a MAXIMUM of TWO active indexes in RAM at any time. If you activate a third, the system will crash.
            2. BALANCING ACTIONS: If you need to create a new index but already have 2 active indexes, you MUST use the 'clear' action on one of the active ones in the same decision batch to prevent a crash.
            3. EFFICIENCY: Avoid unnecessary clearing and re-creating of indexes if the current workload doesn't justify it. Use 'pass' if no structural changes are required.

            OUTPUT FORMAT:
            Return a JSON object matching the requested schema. Output ONLY the raw JSON object. Do NOT include any markdown formatting (like ```json), introductory text, or conversational filler.
            """
            
            user_prompt = f"""
            Current Database State Metrics:
            - Table Scans / Index Hops: Name={name_vis}, Age={age_vis}, Country={country_vis}
            - Query Counts: Name={name_q}, Age={age_q}, Country={country_q}
            - Currently Active Indexes in RAM: {active_graphs}
            
            Analyze the bottlenecks. Provide your optimal structural indexing actions.
            """
            
            try:

                response = client.models.generate_content(
                model='gemini-2.5-flash',
                contents=user_prompt,
                config=types.GenerateContentConfig(
                system_instruction=system_instruction,
                temperature=0.0,
                response_mime_type="application/json",
                response_schema=native_json_schema
                )
                )

                print("\nPARSED:")
                print(response.parsed)

                if response.parsed:
                    structured_data = response.parsed

                    llm_decision = (
                    f"{structured_data['action1']} "
                    f"{structured_data['target1']} "
                    f"{structured_data['action2']} "
                    f"{structured_data['target2']}"
                    )

                    print("\nLLM DECISION:")
                    print(llm_decision)

                    process.stdin.write(llm_decision + "\n")
                    process.stdin.flush()

                else:
                    print("response.parsed is None")
                    process.stdin.write("pass name pass name\n")
                    process.stdin.flush()

            except Exception as e:
                print(f"Error: {str(e)}")
                process.terminate()
                break

if __name__ == "__main__":
    main()