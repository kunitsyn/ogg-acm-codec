dump.inl: codec.cpp codec.h encode.cpp
	.\vorbis.exe > dump.inl

clean:
	del /f dump.inl
