class RoversController < ApplicationController
  def telemetry
    @rover = Rover.find_or_create_by_Name(params[:Name])

    @rover.TelemetryRcvd = Time.now
    @rover.Millis = params[:Millis]
    @rover.GPSData = params[:GPSData]
    @rover.Clearance = params[:Clearance]

    if @rover.save
      render :text => "Success!"
    else
      render :text => "Failure!"
    end
  end

  # GET /rovers
  # GET /rovers.xml
  def index
    @rovers = Rover.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @rovers }
    end
  end

  # GET /rovers/1
  # GET /rovers/1.xml
  def show
    @rover = Rover.find(params[:id])

    respond_to do |format|
      if request.xhr?
        format.html {   render :partial => "rovers/show/partials/show"  }
      else
        format.html # show.html.erb
      end
      format.xml  { render :xml => @rover }
    end
  end

  # GET /rovers/1/edit
  def edit
    @rover = Rover.find(params[:id])
  end

  # PUT /rovers/1
  # PUT /rovers/1.xml
  def update
    @rover = Rover.find(params[:id])

    respond_to do |format|
      if @rover.update_attributes(params[:rover])
        format.html { redirect_to(@rover, :notice => 'Rover was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @rover.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /rovers/1
  # DELETE /rovers/1.xml
  def destroy
    @rover = Rover.find(params[:id])
    @rover.destroy

    respond_to do |format|
      format.html { redirect_to(rovers_url) }
      format.xml  { head :ok }
    end
  end
end
