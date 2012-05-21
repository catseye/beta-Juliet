#!/usr/local/bin/perl -w

# portia.pl - event-parametizing pre-processor for beta-Juliet.
# v2001.02.01 Chris Pressey, Cat's-Eye Technologies.

# Copyright (c)2001, Cat's Eye Technologies.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#   Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 
#   Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
# 
#   Neither the name of Cat's Eye Technologies nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission. 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE. 

### HACK ###

$program = join('',<STDIN>);

while ($program =~ /^.*?alphabet\s+(.*?)\s*\,\s*(.*?)\;/s)
{
  $program = $';
  $alphabet{$1} = [ split(/\s*,\s*/, $2) ];
}

while ($program =~ /^.*?event\s*(.*?)(\,.*?[\;\.])/s
    or $program =~ /^.*?event\s*(.*?)([\;\.])/s)
{
  $program = $';
  $full_event_name = $1;
  $full_subprogram = $2;

  if ($full_event_name =~ /^(.*?)\((.*?)\=(.*?)\)(.*?)$/)
  {
    $pre = $1;
    $var = $2;
    $range = $alphabet{$3};
    $post = $4;

    for($i=0;$i<=$#{$range};$i++)
    {
      $k = $range->[$i];
      $knext = $range->[$i+1];
      $kprev = $range->[$i-1];
      $subprogram = $full_subprogram;
      $subprogram =~ s/\($var\)/$k/ges;
      $subprogram =~ s/\(succ\s*$var\s*\|(.*)\)/$knext || $1/ges;
      $subprogram =~ s/\(pred\s*$var\s*\|(.*)\)/$kprev || $1/ges;
      print "event $pre$k$post" . $subprogram, "\n";
    }
  } else
  {
    print "event $full_event_name" . $full_subprogram, "\n";
  }
}

### END HACK ###
