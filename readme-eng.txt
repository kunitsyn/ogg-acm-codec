** Attention **
This document is the one that translated an original document (Japanese) into English mechanically.
This document after translation is not having a sufficient inspection.
The result as, The contents of an original document are not necessarily being reflected faithfully.


* About Ogg Vorbis CODEC for MSACM

Ogg Vorbis CODEC for MSACM (following vorbis.acm) is the CODEC driver who is actuated on Windows 9x/NT/2000/XP and conformed to MSACM.
Ogg Vorbis technology support is added, to the various software, that use conformance MSACM CODEC.

Ogg Vorbis stream data is becoming so-called VBR (variable bit rate) and even the encoding output of vorbis.acm is VBR also.
However many of the application, that use the interface of MSACM and also MSACM are assuming CBR (constant bit rate).

Several attention is necessary with a compatibility to use vorbis.acm.
Read this document for, Understand and solve compatibility problem in your environment.


* No warranty

Vorbis.acm is no warranty software.
We take no responsibility, granted that,
due to the fact that used this software and wore any damage directly or indirectly.

This software is presented condition with "as is".
Even if faulty, unactuation, differs from specification,
We do not have the responsibility with regard to modification.

The specification of vorbis.acm may be modified without previous notice in the future.
In this case, existing data compatibility are possibly lost.


* Copyright

Ogg Vorbis Audio CODEC
Copyright (C)2001 H.Mutsuki

Xiphophorus libVorbis I 20011231
Copyright (C)2001 Xiphophorus

vorbis.acm is based on libVorbis.
About the package part of vorbis.acm, we possess copyrights.
About the package part of libVorbis, Xiphophorus possess copyrights.

In the case that a source code is included to a package,
Similarly copyrights is possessed.


* Distribution condition

Vorbis.acm is free software.
You may do use, distribution, reproduction etc. freely, unless the contents are modified.
However, If you could report after that about this case it is grateful.

Distribution condition of libVorbis is followed, because vorbis.acm is including libVorbis.
I think that it has no problem if it distributes it without modifying an original archive.
Please refer to COPYING.txt about the distribution condition of detailed libVorbis.

In the case of the release that does not include a source code.
Source code is distributed without a condition fundamentally, if you could contact.

*Distributing only binary in, vorbis.acm of several version in the past a source code was not included.
(Furthermore, it was always distributing it with a source code, before 1.0-beta4. It is because it complies with a LGPL rule.)
There were several reasons about it.
1.It is not able to open it unconditionally, because there is capacity restriction in the site for distribution.
2.It is a time-consuming to delete an unrelated code, because I am diverting other source code.
3.Many users (many of the users that say here are a software developer) did not require a source code.

I do the disclosure, distribution of a source code fundamentally gratuitously in conformity with the mind of open source.
Even that demands an exchange evaluates, if you require support.
Please relax. I have not demanded an exchange in fact.


* Operation environment

Windows 9x/NT/2000/XP in which audio compression manager (following MSACM) was installed.
(Although it should be corresponding after NT3.1 fundamentally. It is not confirming in before Windows 2000.)
Furthermore, the application (Sound Recorder etc.) corresponding to MSACM is necessary to an actual application.


* Installation

Right click "vorbisacm.inf" and select 'install' from the context menu.

Basically restarting of the operating system is not necessary.
However, the software that has already started does not recognize vorbis.acm.
As occasion demands, do the restarting of application or operating system.

** Attention **
In the case that require vorbis.acm to 'possession' due to the actuation of your software and also distribute simultaneously.
Do not instruct the installation of vorbis.acm to an end user.
In this case, I 'do not recommend' vorbis.acm are installed as a global driver strongly.

Application of the 'possession' that says here,
Software that requires vorbis.acm to only to achieve the purpose that playback particular contents.
For example, Game software, multimedia contents etc.
Do not relevance, Thing as the main purpose of playback, preparation, editing, conversion etc.
For example, multimedia player, sound editors, data converter etc.

Generally, essentially unrelated software is installed, it is not liked.

In this case, I recommend that adds vorbis.acm as a local driver and use strongly.
As the result, To avoid the compatible problem, same version needs to be distributed simultaneously, by the version of used for development.
Local driver always has high priority. With a benefit,
Problem such as priority, version compatibility can be prevented.


* Uninstallation

Right click "uninstall.inf" and select 'install' from the context menu.

In the case that vorbis.acm is active in case vorbis.acm is not deleted.
In this case, do the restarting of application or operating system.
after that, do the same operation again, or delete vorbis.acm manually.

The information (priority information, format detailed cache, etc.) that MSACM prepared in the method that uses uninstall.inf are not deleted.
Please delete it manually in the case that, anxious about it.
Furthermore, current version of vorbis.acm does not do the read write registry and, ini file for oneself.


* Development background

Ogg Vorbis is very attractive technology for developer.
Because, Ogg Vorbis has compression performance and quality equivalent to mp3.
Although it is it, 'patent free' and 'royalty free'.

Such attractive interesting technology is offered.
However, Without being related to that, Popularity of Ogg Vorbis in our neighbor is very low, it is very regrettable.

It corresponds to end user software environment was uses without being arranged sufficiently.
Such a present condition to think that it is influencing most largely.

That I am important for us.
Be that, can use the most of the software property that already exists.
Be that, can use right away on the infrastructure that was constructed.
It is nonexistent to achieve our application purpose, with only such requirement that is the standards definition and able to playback with particular player.
It is insufficient as such centripetal force that diffuses this new compression technology.

We adopted this time is the software form called the CODEC driver that conformed to MSACM.

In the recent Windows circumstances, Although it is as the development such as, DirectShow Filter and, each soft original plug-in is prosperous even from MSACM.
(1) Applicable for software of existing property.
(2) Data can be encoded in addition to playback.
MSACM is the good form of convenience to fill this requirement.

*It was to enable the application of Ogg Vorbis technology with the excellent platform called Sound Forge 4.5, that we think most important and did.

I have come to be able to use it without modifying them, on many software that already exist, by conforming to MSACM.
Also, it may be being facing even to use with the multimedia contents, game software etc., because distribution is permitted gratuitously.
(We actually developed it with the purpose originally...)


* Specification

Sorry. now writing...


* About Pseudo CBR

Sorry. now writing...


* Performance

Sorry. now writing...


* Compatibility

Sorry. now writing...


* General restriction

Sorry. now writing...


* Development environment

Sorry. now writing...


* Version history

2002/01/30	Ver 0.0.3.6
Bitrate manage of libVorbis avoided being used.
It was using it with 0.0.3.4 0.0.3.5

2002/01/30	Ver 0.0.3.5
Improvement of the compatibility with some data.
Private beta.

2002/01/30	Ver 0.0.3.4
Improvement of the compatibility with part of application. (modification of a result value of acmStreamSize())
Compatible confirmation work with PlaySound() at Win32(XP/ME).
Document for English-speaking user (readme-eng.txt) was included to a distribution archive. (yet more add it writing)
Other any small modification.

2002/01/19	Ver 0.0.3.3
Improvement of the compatibility with part of application. (modification of a result value of acmStreamSize())
Modification of that the possibly clip noise, that depends to following errors, overflow and inverse sign.
Added more bit rates selection. (quality parameter of libVorbis 0.0 to 1.0 even it per 0.1 units)
Modification of the bit rate of 11KHz, 22KHz.
Other many small modification.

2002/01/11	Ver 0.0.3.2
Base library was changed to 1.0-rc3 (libVorbis I 20011231)
Rewrote a source code substantially. (Therefore possibly bugs was embeded...)
Header size of mode2 was improved. (WMP 7,8 almost became possible playback. I think so, probably...)
Distribution archive was changed to a ZIP format. (convenience to the user other than a Japanese)
Source code was included to a distribution archive.

2001/09/13	Ver 0.0.3.1
Base library was changed to latest.
Sample rate 48KHz was Added.
Quietly renewal...

2001/06/22	Ver 0.0.3.0
Base library was changed to latest.
Provisionally open version...

2001/02/27	Ver 0.0.2.7
Base library was changed to 1.0-beta4 (libVorbis I 20010225)
Vorbis.acm license was modified to free software.
Because by the fact that the license of base library was modified.
Beta test version for ourselves...

2001/01/22	Ver 0.0.2.6
Base library was changed to latest.
WAVEFORMATEX::cbSize of mode2 was larger than a previous version along with the changed of libvorbis.
Speed improved substantially along with the changed of libvorbis.
Document for developper (developer.txt) was included to a distribution archive.

2000/12/14	Ver 0.0.2.5
Base library was changed to latest.
Version information the modification of that that was not correct at 0.0.2.4.

2000/12/13	Ver 0.0.2.4
Base library was changed to latest.
Handling of 11KHz, 22KHz were modified. (attention! I discarded the compatibility with an old version)

2000/12/12	Ver 0.0.2.3
Improvement of the compatibility with part of application. (insert method of padding for Pseudo CBR.)
Modification for, bugs was embeded to mode3, mode3+ at 0.0.2.2.

2000/12/03	Ver 0.0.2.2
Improvement of the compatibility with part of application. (insert method of padding for Pseudo CBR.)
Improvement of the compatibility with part of application. (size calculation at the acmStreamSize())
Speed improved with a side effect (?) when source code was tidied.

2000/11/28	Ver 0.0.2.1
Modification of the problem that time-consumes to the enumeration of a format.
(dumped format details was linked to module)

2000/11/27	Ver 0.0.2.0
Debugged attentively on W9x environment. (I actually do it for the first time...)
Confirm playback normality virtual audio CD, with a W9x version of "CD Kakumei Virtual".
(Kakumei is Japanese word, it is meaning revolution)
Improvement of the compatibility with part of application.
(Modification of a memory manage and result value of acmStreamSize())

2000/11/25	Ver 0.0.1.4
Modification of the memory manage of driver instance.
Because, depends on the following reason.
Operating system does possibly crash end of process.
It is prominent with SoundForge4.5 etc on W9x.

2000/11/24	Ver 0.0.1.3
Modification for, insert location of padding for Pseudo CBR.
Modification for, Problem actuated with SoundRecoder on W9x.
Sample rate 11KHz was removed at temporary.

2000/11/23	Ver 0.0.1.2
Added Pseudo CBR mode (mode1+,mode2+,mode3+)
Time when hangs to format enumeration increased, because new mode added.
(It is prominent with SoundForge4.5 etc.)

2000/11/22	Ver 0.0.1.1
Modification for, Fatal problem actuated with Sound Forge 4.5
(Modified method of the size calculation at the acmStreamSize())

2000/11/21	Ver 0.0.1.0
Modification to mode1 of the name of Original stream compatible
Modification to mode2 of the name of Have independent header
Modification to mode3 of the name of Have no codebook header
To solve the compatibility regarding the size of WAVEFORMATEX modification of header specification.
(Therefore the data and compatibility that prepared it with an old version were lost.)
Modification from closed beta to public beta.
Reference translation (lesserj.txt) of LGPL was added to archive.

2000/11/19	Ver 0.0.0.2
Modification for, encoding problem at Have independent header
Modification for, code book selecting at 22050Hz,11025Hz

2000/11/18	Ver 0.0.0.1
Added Have independent header
Added Have no codebook header
Beta test version for ourselves

2000/11/12	Ver 0.0.0.0
Support only Original stream compatible
Closed development version


* From developer

Sorry. now writing...
