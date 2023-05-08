from flask import Flask, request, render_template, jsonify
import sys

app = Flask(__name__)

def parse_data(data):
    generations = {}
    current_generation = None
    for line in data:
        line = line.strip()
        if line.startswith("Generation:"):
            # Start of a new generation
            current_generation = int(line.split(':')[1].strip())
        elif line.startswith("Fitness:"):
            # Fitness value
            generations[current_generation] = {'fitness': int(line.split(':')[1].strip()), 'population': []}
        elif line.startswith("Chromosome:"):
            # Complex numbers
            nums = line[12:].strip('[]').replace(' ', '').replace('+-', '-').split(',')
            nums = [1 - float(num[0:7])**2 for num in nums]
            generations[current_generation]['population'].append(nums)

    return generations

@app.route('/')
def index():
    return render_template('index.html', max_generation=max_generation, max_index=max_index)

@app.route('/chromosome', methods=['GET'])
def get_chromosome():
    generation = int(request.args.get('generation'))
    index = int(request.args.get('index'))

    if generation in parsed_data and 'population' in parsed_data[generation] and index < len(parsed_data[generation]['population']):
        chromosome = parsed_data[generation]['population'][index]
        return jsonify({'chromosome': chromosome, 'fitness': parsed_data[generation]['fitness']})
    else:
        return jsonify({'error': 'Invalid generation or index'})

if __name__ == '__main__':
    # Check if a filename argument is provided
    if len(sys.argv) < 2:
        print("Please provide a filename as an argument.")
        sys.exit(1)

    # Get the filename from command-line argument
    filename = sys.argv[1]

    # Read data from the file
    try:
        with open(filename, 'r') as file:
            input_data = file.readlines()
    except IOError:
        print("Error opening the file:", filename)
        sys.exit(1)

    # Parse the data
    parsed_data = parse_data(input_data)
    max_generation = len(parsed_data) - 1
    max_index = len(parsed_data[1]['population']) - 1

    app.run()
