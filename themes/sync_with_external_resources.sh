#!/bin/bash
set -e

while IFS=$'\t\r' read -r name directory license_url cards_url
do
    echo "$name @ $cards_url"
    mkdir -p "$directory"
    curl -s "$cards_url" > "$directory/cards.svg"
    curl -s "$license_url" > "$directory/README"
done < <(jq ".[] | [.name, .dir, .license_url, .cards_url] | @tsv" ./external_card_resources.json -r)

for filename in $(find -name "*.svg"); do
    sed -i -f fix_cards.sed "$filename"
done
