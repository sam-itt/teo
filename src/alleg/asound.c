/*
 *    TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *    TTTTTTTTTTTTTT  EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EEEEEEEEEE      OO          OO
 *          TT        EEEEEEEEEE      OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EE              OO          OO
 *          TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *          TT        EEEEEEEEEEEEEE  OOOOOOOOOOOOOO
 *
 *                  L'�mulateur Thomson TO8
 *
 *  Copyright (C) 1997-2012 Gilles F�tis, Eric Botcazou, Alexandre Pukall,
 *                          J�r�mie Guillaume, Samuel Devulder
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *  Module     : alleg/sound.c
 *  Version    : 1.8.1
 *  Cr�� par   : Eric Botcazou avril 1999
 *  Modifi� par: Eric Botcazou 24/09/2001
 *               Samuel Devulder 23/03/2010
 *               Fran�ois Mouret 08/2011
 *
 *  Gestion de l'�mulation sonore du TO8.
 */


#ifndef SCAN_DEPEND
   #include <stdio.h>
   #include <string.h>
   #include <allegro.h>
#endif

#include "to8.h"


static AUDIOSTREAM *stream;
static int sound_freq;
static int sound_buffer_size;
static unsigned char *sound_buffer;
static int last_index;
static unsigned char last_data;



/* StartSound:
 *  Lance le streaming audio.
 */
void StartSound(void)
{
    voice_start(stream->voice);
}



/* StopSound:
 *  Arr�te le streaming audio.
 */
void StopSound(void)
{
    voice_stop(stream->voice);
}



/* GetVolume:
 *  Lit le volume du streaming audio.
 */
int GetVolume(void)
{
    return voice_get_volume(stream->voice);
}



/* SetVolume:
 *  Fixe le volume du streaming audio.
 */
void SetVolume(int val)
{
    voice_set_volume(stream->voice, val);
}



/* PutSoundByte:
 *  Place un octet dans le tampon arri�re du streaming audio.
 */
static void PutSoundByte(unsigned long long int clock, unsigned char data)
{
    int index=(clock%TO8_CYCLES_PER_FRAME)*sound_freq/TO8_CPU_FREQ;

    /* Dans le cas o� le nombre de cycles �x�cut�s pendant une frame d�passe la valeur
       th�orique, on bloque l'index � sa valeur maximale */
    if (index < last_index)
        index=sound_buffer_size;

    memset (&sound_buffer[last_index], last_data, index-last_index);

    last_index=index;
    last_data=data;
}



/* PlaySoundBuffer:
 *  Echange les tampons avant et arri�re du streaming audio.
 */
void PlaySoundBuffer(void)
{
    char *buffer_ptr;

    /* Pour �viter les "clac" si ralentissement */
    if (last_index==0) last_data=0;

    /* on remplit la fin du buffer avec le dernier byte d�pos� */
    memset (&sound_buffer[last_index], last_data, sound_buffer_size-last_index);

    last_index=0;

    while ((buffer_ptr=get_audio_stream_buffer(stream)) == NULL)
		rest(10); /* 1/2 vbl pour ne pas utiliser 100% de CPU */

    memcpy(buffer_ptr, sound_buffer, sound_buffer_size);
    
    free_audio_stream_buffer(stream);
}



/* voice_get_position_callback:
 *  Helper pour d�tecter le bon fonctionnement du streaming audio.
 */
static void voice_get_position_callback(void)
{
    if (voice_get_position(stream->voice))
        teo.sound_enabled=TRUE;
}



/* InitSound:
 *  Initialise le module de streaming audio.
 */
void InitSound(int freq)
{
    sound_freq = freq;
    sound_buffer_size = sound_freq/TO8_FRAME_FREQ;
    sound_buffer = malloc(sizeof(unsigned char)*sound_buffer_size);

    to8_PutSoundByte=PutSoundByte;

    if (!teo.sound_enabled)
        return;

    teo.sound_enabled=FALSE;

    printf(is_fr?"Initialisation du son...":"Sound initialization...");

    /* pas de compensation de volume */
    set_volume_per_voice(0);

    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) == 0)
    {
         /* test de fonctionnement du streaming */
         stream=play_audio_stream(sound_buffer_size, 8, FALSE, sound_freq, 128, 128);
         rest_callback(100, voice_get_position_callback);  /* 100 ms */
         voice_stop(stream->voice);
    }

    printf(teo.sound_enabled ? "ok\n" : (is_fr?"erreur\n":"error\n"));
}

