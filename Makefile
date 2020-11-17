SOURCES=src/lightfollower.ino

all: ${SOURCES}
	arduino --upload ${SOURCES}
