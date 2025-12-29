TARGET=AudioPlay

CXX=g++

source = main.cpp

all: $(TARGET)
	$(CXX) $*(source) -o $(TARGET)

clean:
	@echo "cleaning..."
	rm $(TARGET)
