import os
from flask import Flask, jsonify


FLAG = os.getenv("flag") if os.getenv("flag") else "ACSC{THIS_IS_FAKE}"

app = Flask(__name__)
emojis = []


@app.route("/", methods=["GET"])
def root():
    return FLAG


@app.route("/v1/get_emojis")
def get_emojis():
    output = {"data": emojis}
    return jsonify(output)


def initialize():
    with open("./emojis.txt", "r") as f:
        e = f.read()
    for i in e.split("\n"):
        if i.strip() == "":
            continue
        name, emoji = i.split(" ")
        emojis.append({
            "name": name,
            "emoji": emoji
        })


initialize()
app.run("0.0.0.0", 8000, debug=False)
