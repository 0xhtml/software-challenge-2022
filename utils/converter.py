"""Convert replay xml files to state txt files."""

import argparse
import glob
import gzip

import defusedxml.ElementTree


def _parse_board(element) -> list:
    row = ["0"] * 8
    board = []
    for _ in range(8):
        board.append(row.copy())

    for entry in element.iter("entry"):
        coordinates = entry.find("coordinates")

        x = int(coordinates.get("x"))
        y = int(coordinates.get("y"))

        piece = entry.find("piece")

        piecetype = piece.get("type")

        if piece.get("team") == "TWO":
            piecetype = piecetype.lower()

        count = piece.get("count")

        board[x][y] = f"{piecetype[0]}{count}"

    return board


def _encode_board(board: list) -> str:
    board_str = ""

    for x in range(8):
        num = 0

        for y in range(8):
            f = board[x][y]

            if f == "0":
                num += 1
                continue

            if num != 0:
                board_str += str(num)
                num = 0

            p = f[0]

            board_str += p

            if f[1] == "2":
                board_str += "."

        if num != 0:
            board_str += str(num)

        if x != 7:
            board_str += "/"

    return board_str


def _parse_score(element) -> str:
    score = [0, 0]

    for entry in element.iter("entry"):
        if entry.find("team").text == "ONE":
            score[0] = int(entry.find("int").text)
        elif entry.find("team").text == "TWO":
            score[1] = int(entry.find("int").text)

    return f"{score[0]}-{score[1]}"


def convert_all():
    """Convert all replay xml files."""
    with open(args.output, "w") as state_file:
        for xml_filename in glob.glob(f"{args.input}*.xml*"):
            print(f"Converting {xml_filename.split('/')[-1]}")

            if xml_filename.endswith(".xml.gz"):
                with gzip.open(xml_filename, "rt") as xml_file:
                    root = defusedxml.ElementTree.fromstring(xml_file.read())
            elif xml_filename.endswith(".xml"):
                with open(xml_filename, "r") as xml_file:
                    root = defusedxml.ElementTree.fromstring(xml_file.read())
            else:
                continue

            if root.find(".//score[@cause!='REGULAR']"):
                continue

            win = 0.5
            for winner in root.iter("winner"):
                if winner.get("team") == "ONE":
                    win = 1
                else:
                    win = 0

            states = [x for x in root.iter("state")]

            if len(states) == 0:
                continue

            for state in states[1:]:
                board = _parse_board(state.find("board"))

                board_str = _encode_board(board)
                turn = int(state.get("turn"))

                score = _parse_score(state.find("ambers"))
                state_file.write(f"{board_str} {score} {turn} {win}\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument("input")
    parser.add_argument("output")

    args = parser.parse_args()

    convert_all()
