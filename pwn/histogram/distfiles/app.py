import flask
import tempfile
import subprocess

app = flask.Flask(__name__)

@app.route('/')
def inedx():
    return flask.render_template('index.html')

@app.route('/api/histogram', methods=['POST'])
def histogram():
    csv = flask.request.files.get('csv')
    with tempfile.NamedTemporaryFile() as t:
        csv.save(t.name)
        result = subprocess.run(["./histogram.bin", t.name], stdout=subprocess.PIPE)
    return result.stdout

if __name__ == '__main__':
    app.run()
