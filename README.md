My pet language

Some language implementations have an image saving/loading feature that I think is neat.
A lot of software is image based (e.g. Excel, SQL, CL SLIME), and there can be some downsides.
The worse that I've run into is mismatch between the image and the source code.

This happens to me in Common Lisp SLIME where I'm working on an image, but I forget what
code I've sent to the image, so if I wanted to recreate this image from source code, it wouldn't
be possible. If I simply wrote all my code in a way that it could easily be loaded into an image
from scratch, it lessens the benefit of having the image in the first place.

It seems like SmallTalk has done it best where the source code of the image can be exported.
SQL Server has something like this too, but it exposes the flaw. The source code generated
never matches with how you wrote the code to begin with. There's always some formatting difference
between the way people program and the source that is generated from the images. The way people
format their code is important and shouldn't be removed.

I think there could be room for my pet to improve this space. The perfect implementation to me
would be something that is entirely reliant on the image (this eliminates the mismatch between source
code and the image), and can export source code that is identical to what the programmer coded.

Using images instead of fancy macros and compiling techniques could reach a nice middle ground where
a language can be extended and work can be done outside of run time. Distributing code would be 
distributing the image (this isn't a new idea by any means). There could be routines run at 
"image generation time" that could save time during run time. The less compilation that happens 
the easier it would be to keep the integrity of the image. The more compilation happens the more often
you may need (or want) to re-compile parts of an image. For example, if you have a macro that you've 
used in 10 different functions, then you update that macro, you will likely want to go and recompile
those 10 functions so they get the updated source. If you don't rely on macros and instead take a run-time
performance hit, it will be easier to maintain the image (the macro would be a function call instead).

LISP is pretty simple, so that makes for a good language to base off of.
