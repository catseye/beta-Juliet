#!/usr/bin/env perl

# bj.pl - beta-Juliet interpreter/simulator in Perl 5
# v2001.02.01 Chris Pressey, Cat's Eye Technologies.

# Copyright (c)2001-2012, Chris Pressey, Cat's Eye Technologies.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notices, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notices, this list of conditions, and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#  3. Neither the names of the copyright holders nor the names of their
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

### GLOBALS ###

$line = '';
$token = '';
$errors = 0;
$serial = 0;

### SCANNER ###

sub err
{
  my $msg = shift;
  print "ERROR: $msg (token = $token).\n";
  $errors++;
}

sub scan
{
RESTART:
  if ($line eq '')
  {
    $line = <INFILE>;
    goto RESTART;
  }
  if ($line =~ /^\s+/)
  {
    $line = $';
    goto RESTART;
  }
  if ($line =~ /^\/\/.*?$/)
  {
    $line = '';
    goto RESTART;
  }
  if ($line =~ /^([A-Za-z\-]+)/)
  {
    $token = $1;
    $line = $';
  }
  elsif ($line =~ /^(\d+\.*\d*)/)
  {
    $token = $1;
    $line = $';
  }
  elsif ($line =~ /^(.)/)
  {
    $token = $1;
    $line = $';
  }
}

### PARSER ###

sub WhenTerm
{
  my @l = ();
  while($token eq 'when')
  {
    scan();
    my $name1 = $token; my $name2 = '';
    scan();
    if($token eq '>')
    {
      scan();
      $name2 = $token;
      scan();
      push @l, $name1;
      push @l, $name2;
    } else
    {
      err "Need state expression on right side of 'when'";
    }
  }
  return \@l;
}

sub Property
{
  my $name = shift;
  my $time = '';
  my $byname = '';
  my $immediate = 0;
  my $when;
  if ($token eq 'caused')
  {
    scan();
    $time = $token;
    scan();
    $byname = $token;
    scan();
    if ($token eq 'when')
    {
      $when = WhenTerm();
    }
    if ($time eq 'before')
    {
      push @{$before{$byname}}, $name;
      $when_before{$byname}{$name} = $when if defined $when;
    }
    if ($time eq 'after' or $time eq 'by')
    {
      push @{$after{$byname}}, $name;
      $when_after{$byname}{$name} = $when if defined $when;
    }
  }
  elsif ($token eq 'causes')
  {
    scan();
    my $subj = $token;
    scan();
    if ($token eq 'immediately')
    {
      scan();
      $immediate = 1;
    }
    if ($token eq 'when')
    {
      $when = WhenTerm();
    }
    if ($immediate)
    {
      push @{$before{$name}}, $subj;
      $when_before{$name}{$subj} = $when if defined $when;
    } else
    {
      push @{$after{$name}}, $subj;
      $when_after{$name}{$subj} = $when if defined $when;
    }
  }
  elsif ($token eq 'duration')
  {
    scan();
    $dur = 0 + $token;
    scan();
    $dur /= 1000 if $token eq 'ms';
    $dur *= 60 if $token eq 'm';
    $dur *= 3600 if $token eq 'h';
    $dur *= (24*3600) if $token eq 'd';
    $dur{$name} = $dur;
    scan();
  }
  else { err "Not a legal property"; }
}

sub Event
{
  my $name = '';
  if($token eq 'event')
  {
    scan();
    $name = $token;
    scan();
    while ($token eq ',')
    {
      scan();
      Property($name);
    }
    $event{$name} = $name;
    $last{$name} = 0;
    $ltime{$name} = '' . localtime(time());
  } else
  {
    err "Missing event";
  }
}

sub betaJuliet
{
  Event();
  while($token eq ';')
  {
    scan();
    Event();
  }
  if($token ne '.') { err "Missing period"; }
}

### RUNTIME ###

%ready = ();

sub DumpEvents
{
  foreach $k (sort keys %event)
  {
    print "$k: ";
    if ($last{$k} > 0)
    {
      print "(last fired at " . $ltime{$k} . ") ";
    } else
    {
      print "(never fired) "
    }
    if (exists $ready{$k} and $ready{$k})
    {
      print " (ready to fire)";
    }
    print "\n";

    if (exists $before{$k})
    {
      $bk = join(', ', @{$before{$k}});
      print "  before:   $bk\n";
    }
    print "  duration: $dur{$k}\n" if exists $dur{$k};
    if (exists $after{$k})
    {
      $ak = join(', ', @{$after{$k}});
      print "  after:    $ak\n";
    }
  }
}

sub fire
{
  my $name = shift;
  my $q;
  if (exists $event{$name})
  {
    if (exists $before{$name})
    {
      foreach $q (@{$before{$name}})
      {
        if (exists $when_before{$name}{$q})
        {
          my @l = @{$when_before{$name}{$q}};
          my $b = 1;
          while($#l >= 0)
          {
            my $first = shift @l;
            my $second = shift @l;
            $b = ($b and ($last{$first} > $last{$second}));
          }
          fire($q) if $b;
        } else
        {
          fire($q);
        }
      }
    }
    print "Firing '$name'...\n";
    $ready{$name} = 0;
    if (exists $after{$name})
    {
      foreach $q (@{$after{$name}})
      {
        if (exists $when_after{$name}{$q})
        {
          my @l = @{$when_after{$name}{$q}};
          $ready{$q} = 1 if $last{$l[0]} > $last{$l[1]};
        } else
        {
          $ready{$q} = 1;
        }
      }
    }
    $ltime{$name} = '' . localtime(time());
    $last{$name} = ++$serial;
  } else
  {
    print "Non-existant event '$name'!\n";
  }
}

sub LoadFile
{
  my $filename = shift;
  $errors = 0;
  err("Can't open $filename") if not open INFILE, "$filename";
  if ($errors == 0)
  {
    scan();
    betaJuliet();
    close INFILE;
  }
  if ($errors > 0)
  {
    print "There were $errors errors.\n";
  }
}

sub UserInterface
{
  my $cmd = '';

  while(uc($cmd) ne 'Q')
  {
    print "beta-Juliet> ";
    $cmd = <STDIN>;
    chomp $cmd;

    if (uc($cmd) eq 'D')
    {
      DumpEvents();
    }
    elsif (uc($cmd) eq 'F')
    {
      my $k = '';
      foreach $k (keys %ready)
      {
        fire($k) if $ready{$k};
      }
    }
    elsif (uc($cmd) eq 'R')
    {
      my $done = 0;
      my $k = '';
      while (not $done)
      {
        foreach $k (keys %ready)
        {
          fire($k) if $ready{$k};
        }
        $done = 1;
        foreach $k (keys %ready) { $done = 0 if $ready{$k}; }
        # sleep 1 if not $done;
      }
    }
    elsif (uc($cmd) eq 'Q')
    {
      print "Quitting.\n";
    }
    elsif (uc($cmd) eq 'NEW')
    {
      %ready = ();
      %event = ();
      %before = ();
      %after = ();
      %when_before = ();
      %when_after = ();
      %dur = ();
      %last = ();
      %ltime = ();
      print "All events erased.\n";
    }
    elsif (-r $cmd)
    {
      print "Loading $cmd...\n";
      # 2001.02.01 Chris Pressey: adding support for Portia
      if ($cmd =~ /\.pbj$/)
      {
        # print "(Expanding Portia)\n";
        # print `perl portia.pl <$cmd`;
        LoadFile("perl portia.pl <$cmd |");
      } else
      {
        LoadFile($cmd);
      }
    }
    elsif (exists $event{$cmd})
    {
      if (exists $ready{$cmd} and $ready{$cmd})
      {
        $ready{$cmd} = 0;
        print "event $cmd has been made unready to fire.\n";
      } else
      {
        $ready{$cmd} = 1;
        print "event $cmd is now ready to fire.\n";
      }
    }
    else
    {
      print "Unrecognizable command '$cmd'";
      print '

NEW = Erase All Events
FileName = Load beta-Juliet File
EventName = Toggle Event Ready
F = Fire all Ready Events (Single Step)
R = Repeated Fire (Run)
D = Dump Event Status
Q = Quit beta-Juliet Simulator

';
    }
  }
}

### MAIN ###

print "Cat's-Eye Technologies' beta-Juliet Interpreter v2001.02.01.\n";
UserInterface();

### END ###
