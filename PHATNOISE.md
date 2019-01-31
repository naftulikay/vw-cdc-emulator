# PhatNoise

I personally have a 2004 VW Golf, and had been using a PhatNoise media system, which emulates a CD changer but allows
arbitrary media playback via a hard drive containing music.

## System Caveats

The PhatNoise company was bought and destroyed in the
early 2000's, and the system is extremely finnicky:

 - Only IDE hard drives are supported. IDE SSDs don't appear to work.
 - Hard drives must apparently have secret data located in certain sectors of the disk for the PhatNoise system to
   recognize hard drives as legitimate.
 - Synchronizing from computer to hard drive occurs over a USB 2.0 to IDE dock.
 - The synchronization scheme is non-trivial:
   - There is a complicated on-disk binary database structure with some parts in little-endian and some in big-endian.
   - Playlists are signed with a 512-bit RSA private key.
   - The sync software is only available for Windows XP.
   - I was able to successfully write some Python [and Rust][phatnoise.rs] which was able to do most of the
     synchronization on its own.
 - I dropped the hard disk last year and had to replace it with a new drive and it took about 8 hours to get everything
   working again.
 - I tried to get a 250GiB IDE hard drive to work, lost a lot of time, and failed in this pursuit.

## System Benefits

As frustrating as the PhatNoise system can be at times, the technology was fairly revolutionary for its time and even
today:

 - The entire system can be controlled without taking one's eyes off of the road, as it works via audio prompts.
 - The music library can be browsed by pressing the various CD buttons, which are numbered one through six.
 - Browsing through playlists, albums, artists, and genres use voice prompts to indicate the currently selected item.
   The system will announce "My Cool Playlist," "AC/DC," "Classic Rock," or "A Trick of the Tail" for playlists,
   artists, genres, and albums, respectively.
 - Holding down the next/previous track button in a given mode will quickly browse through the various letters of the
   alphabet for whatever mode it is in, e.g. "A," "B," "C," etc. allowing quick scanning to a particular letter to then
   search within for the desired playlist/artist/album/genre.
 - Each disk enters a different browsing mode:
   1. Pressing Disk 1 is essentially the <kbd>Enter</kbd> key to select whatever is currently in focus.
   2. Pressing Disk 2 enters playlist select mode.
   3. Pressing Disk 3 enters album select mode.
   4. Pressing Disk 4 enters artist select mode.
   5. Pressing Disk 5 enters genre select mode.
   6. Pressing Disk 6 allows controlling settings of the system.
 - By default, whatever is currently being browsed over will be automatically selected within a couple seconds with an
   audible noise confirming the selection.

Being able to seek through a very large, ~60GiB music library without taking one's eyes off of the road is a godsend,
as I really love to drive. Having to click virtual buttons on a phone feels dangerous and is error-prone. PhatNoise
makes all of this fairly painless and efficient.

## Motivation

My motivation in creating this VW CD Changer Emulator is to make a simple embedded board which will allow me to build
my own PhatNoise system with complete control over the head unit. The architecture I'm currently working with will
consist of:

 1. Hopefully, a _very_ minimal Arduino board, as the only physical requirements are four pins for [SPI][spi]
    communication with the head unit, and UART or a Serial Port for external communication with an application driving
    the head unit and responding to key presses/events.
 1. A Raspberry Pi or other small, low-power computer which will bidirectionally communicate with the Arduino board
    over UART/Serial.
 1. A daemon written in [Rust](https://rust-lang.org) for streaming analog audio to the head unit and controlling media
    playback.
 1. A voltage regulator and battery, keying off of the 12V+ DC accessory power supply, which is only on when the key is
    turned to or past the accessory switch.

Once this very simple setup has been achieved, other experiments may follow:

 - Put a Bluetooth receiver on the Raspberry Pi and write an Android application which will be able to communicate with
   the Raspberry Pi from my phone.
 - Put an LTE antenna and SIM card for roaming internet access.
 - Put one or two WiFi antennas, one as a client to be able to connect to my home WiFi and one as a server for in-car
   communication.
 - Use [Syncthing](https://syncthing.net/) to keep my music library in sync on the Raspberry Pi and my other home devices.

Having a full computing platform in the back of the car will enable lots of other exciting experiments and projects.

 [serial]: https://en.wikipedia.org/wiki/Serial_port
