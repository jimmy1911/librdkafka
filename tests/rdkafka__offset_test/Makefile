target=rdkafka_test

all: start  build_src end

build_src:
	make -C src
start:
	rm -rf output
	mkdir -p output
	mkdir -p output/log
	mkdir -p output/bin
	mkdir output/conf
	cp -r ./conf output/
	cp ./scripts/load.sh output/
	cp ./scripts/start.sh output/
end:
	cp -r ./src/${target} output/bin/

clean:
	make -C src clean
	rm -rf output
